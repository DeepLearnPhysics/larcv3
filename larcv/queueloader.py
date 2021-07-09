import sys,time,os,signal
import numpy
import threading
import random


import larcv
from . batch_pydata   import batch_pydata
from . larcv_io_enums import RandomAccess
from . larcv_writer   import larcv_writer

class queue_interface(object):

    def __init__(self, verbose=False, random_access_mode="random_blocks", seed=None):
        '''init function

        Not much to store here, just a dict of dataloaders and the keys to access their data.

        Queue loaders are manually triggered IO, not always running, so
        '''
        object.__init__(self)
        self._queueloaders  = {}
        self._data_keys     = {}
        self._dims          = {}
        self._verbose       = verbose
        self._random_access = RandomAccess[random_access_mode]
        self._minibatch_size = {}

        self._queue_prev_entries = {}
        self._queue_next_entries = {}
        self._count = {}

        if seed is not None:
            random.seed(seed)
            numpy.random.seed(seed)

        self._warning = True

        self._writer = None

    def no_warnings(self):
        self._warning = False

    def get_next_batch_indexes(self, mode, minibatch_size):

        # Using the random_access parameter, determine which entries to read:

        # In this function, we promote the "next" to "prev".  Do that first:
        if mode not in self._queue_next_entries:
            self._queue_next_entries[mode] = None
        self._queue_prev_entries[mode] = self._queue_next_entries[mode]
        # (If this has not been run, they will both be set to None)

        if self._random_access == RandomAccess.serial_access:
            # Figure out the last batch's highest entry:
            if mode in self._queue_prev_entries:
                if self._queue_prev_entries[mode] is not None:
                    last_entry = numpy.max(self._queue_prev_entries[mode])
                else:
                    last_entry = -1
            else:
                last_entry = -1

            next_last_entry = minibatch_size + last_entry + 1

            n_entries = self._queueloaders[mode].fetch_n_entries()

            if next_last_entry < n_entries:
                next_entries = numpy.arange(minibatch_size, dtype=numpy.int32) + last_entry + 1
            else:
                # Create an array to cover the entries till the last one ...
                next_entries_a = numpy.arange(n_entries - 1 - last_entry, dtype=numpy.int32) + last_entry + 1
                # ... and one array for the leftover entries, starting back from zero
                next_entries_b = numpy.arange((last_entry + 1 + minibatch_size) % n_entries, dtype=numpy.int32)
                # Finally concatenate the two arrays
                next_entries = numpy.concatenate((next_entries_a, next_entries_b))

        elif self._random_access == RandomAccess.random_blocks:
            # How many entries are there?
            n_entries = self._queueloaders[mode].fetch_n_entries()
            # The number of available choices is the number of entries - minibatch_size - 1:
            n_choices = n_entries - minibatch_size - 1
            start_entry = numpy.random.randint(low=0, high=n_choices, size=1)
            next_entries = numpy.arange(minibatch_size) + start_entry

        else:  # self._random_access == RandomAccess.random_events
            # Choose randomly, but require unique indexes:
            n_entries = self._queueloaders[mode].fetch_n_entries()
            next_entries = random.sample(range(n_entries), minibatch_size)


        self._queue_next_entries[mode] = next_entries

        return next_entries


    def set_next_index(self, mode, entry):
        '''
        Set the next entry to be read. Only works in serial_access mode.

        Argumements:
            mode {str} -- The mode of training to store this threadio under (typically "train" or "TEST" or similar)
            entry {int} -- The next entry to be read
        '''

        if mode not in self._queue_prev_entries:
            throw('Need to call prepare_manager first.')

        if self._random_access != RandomAccess.serial_access:
            throw('set_next_entry can only be called in serial_access mode.')

        self._queue_next_entries[mode] = [entry - 1.]
        return



    def prepare_manager(self, mode, io_config, minibatch_size, data_keys, color=None):
        '''Prepare a manager for io

        Creates an instance of larcv_threadio for a particular file to read.

        Arguments:
            mode {str} -- The mode of training to store this threadio under (typically "train" or "TEST" or similar)
            io_config {dict} -- the io config dictionary.  Required keys are: 'filler_name', 'verbosity', and 'filler_cfg'
            data_keys_override {dict} -- If desired, you can override the keys for dataacces,

        Raises:
            Exception -- [description]
        '''


        if mode in self._queueloaders:
            raise Exception("Can not prepare manager for mode {}, already exists".format(mode))

        # Check that the required keys are in the io config:
        for req in ['filler_name', 'verbosity', 'filler_cfg']:
            if req not in io_config:
                raise Exception("io_config for mode {} is missing required key {}".format(mode, req))


        start = time.time()

        # Initialize and configure a manager:
        io = larcv_queueio()
        io.configure(io_config)
        self._queueloaders.update({mode : io})
        self._minibatch_size[mode] = minibatch_size

        # Queue loaders are manually triggered IO, not always running, so
        # there is no "start_manager" function.  Everything is manual.
        # First, tell it what the entries for the first batch to read:

        # start = time.time()
        # print(self._queueloaders[mode].is_reading())
        self.prepare_next(mode)
        # end = time.time()

        # Then, we promote those entries to the "current" batch:
        while self._queueloaders[mode].is_reading():
            time.sleep(0.05)

        status = self._queueloaders[mode].is_reading()
        io.pop_current_data()
        io.next(store_entries=True,store_event_ids=True)

        # Note that there is no "next" data pipelined yet.

        # Store the keys for accessing this datatype:
        self._data_keys[mode] = data_keys

        # Read and save the dimensions of the data:
        self._dims[mode] = {}
        for key in self._data_keys[mode]:
            self._dims[mode][key] = self._queueloaders[mode].fetch_data(self._data_keys[mode][key]).dim()

        end = time.time()
        self.prepare_next(mode)

        # Print out how long it took to start IO:
        if self._verbose:
            sys.stdout.write("Time to start {0} IO: {1:.2}s\n".format(mode, end - start))


        return


    def prepare_writer(self, io_config, output_file=None):

        if self._writer is not None:
            raise Exception("queue_interface doesn't yet support multiple writers.")

        # This only supports batch datasize of 1.  We can check that the reading instance
        # Has only size 1 by looking at the dims.

        key =list(self._data_keys['primary'].items())[0][0]
        if self._dims['primary'][key][0] != 1:
            raise Exception("To use the writing interface, please set batch size to 1.")

        # The writer is not an instance of queueIO but rather an instance of larcv_writer.

        # It configures a process to copy input to output and add more information as well.

        self._writer = larcv_writer(io_config, output_file)

        pass


    def prepare_next(self, mode, set_entries = None):
        '''Set in motion the processing of the next batch of data.

        Triggers the queue loader to start reading the next set of data


        '''
        # Which events should we read?
        if set_entries is None:
            set_entries = self.get_next_batch_indexes(mode, self._minibatch_size[mode])

        self._queueloaders[mode].set_next_batch(set_entries)
        self._queueloaders[mode].prepare_next()

        self._count[mode] = 0

        return
        # return threading.Thread(target=self._queueloaders[mode].batch_process).start()

        # self._queueloaders[mode].next(store_event_ids=True, store_entries=True)

        # return

    def fetch_minibatch_data(self, mode, pop=False, fetch_meta_data=False, data_shape=None, channels="last"):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._queueloaders['train'].fetch_data(keyword_label).dim() as an example

        if self._count[mode] != 0:
            if self._warning:
                print("Calling fetch_minibatch_data without calling prepare_next. This will not give new data.")
                print("To quiet this warning, call prepare_next before fetch_minibatch_data or call queueloader.no_warnings()")

        if pop:
            # This function will pop the data
            while self._queueloaders[mode].is_reading():
                time.sleep(0.001)
            self._queueloaders[mode].pop_current_data()
        else:
            if self._warning:
                print("Calling fetch_minibatch_data with pop = False.  This will give you the same data as last time.")
                print("To quiet this warning, call queueloader.no_warnings()")

        self._queueloaders[mode].next(store_entries=fetch_meta_data, store_event_ids=fetch_meta_data)
        this_data = {}

        self.fetch_minibatch_dims(mode)

        for key in self._data_keys[mode]:
            # self._dims[mode][key] = self._queueloaders[mode].fetch_data(self._data_keys[mode][key]).dim()
            this_data[key] = self._queueloaders[mode].fetch_data(
                self._data_keys[mode][key]).data(
                shape=data_shape, channels=channels)
            # this_data[key] = numpy.reshape(this_data[key], self._dims[mode][key])

        if fetch_meta_data:
            this_data['entries'] = self._queueloaders[mode].fetch_entries()
            this_data['event_ids'] = self._queueloaders[mode].fetch_event_ids()

        self._count[mode] += 1

        return this_data

    def fetch_minibatch_dims(self, mode):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._queueloaders['train'].fetch_data(keyword_label).dim() as an example
        return self._dims[mode]

    # def is_active(self):
    #     _is_active = False
    #     for mode in self._queueloaders:
    #         _is_active = _is_active and self._queueloaders[mode].

    def stop(self):

        if self._writer is not None:
            self._writer.finalize()

    def size(self, mode):
        # return the number of images in the specified mode:
        return self._queueloaders[mode].fetch_n_entries()


    def is_reading(self, mode):
        return self._queueloaders[mode].is_reading()


    def write_output(self, data, datatype, producer, entries, event_ids):
        if self._writer is None:
            raise Exception("Trying to write data with no writer configured.  Abort!")


        self._writer.write(data=data, datatype=datatype, producer=producer, entries=entries, event_ids=event_ids)

        return


class larcv_queueio (object):

    _instance_m={}

    @classmethod
    def exist(cls,name):
        name = str(name)
        return name in cls._instance_m

    @classmethod
    def instance_by_name(cls,name):
        return cls._instance_m[name]

    def __init__(self):
        self._proc = None
        self._name = ''
        self._verbose = False
        self._read_start_time = None
        self._read_end_time = None
        self._cfg_file = None
        self._storage = {}
        self._event_entries = None
        self._event_ids = None

    def reset(self):
        while self.is_reading(): time.sleep(0.01)
        if self._proc: self._proc.reset()

    def __del__(self):
        try:
            self.reset()
        except AttrbuteError:
            pass

    def configure(self,cfg, color=0):
        # if "this" was configured before, reset it
        if self._name: self.reset()

        # get name
        if not cfg['filler_name']:
            sys.stderr.write('filler_name is empty!\n')
            raise ValueError

        # ensure unique name
        if self.__class__.exist(cfg['filler_name']) and not self.__class__.instance_by_name(cfg['filler_name']) == self:
            sys.stderr.write('filler_name %s already running!' % cfg['filler_name'])
            return
        self._name = cfg['filler_name']

        # get QueueProcessor config file
        self._cfg_file = cfg['filler_cfg']
        if not self._cfg_file or not os.path.isfile(self._cfg_file):
            sys.stderr.write('filler_cfg file does not exist: %s\n' % self._cfg_file)
            raise ValueError

        # set verbosity
        if 'verbosity' in cfg:
            self._verbose = bool(cfg['verbosity'])

        # configure thread processor
        self._proc = larcv.QueueProcessor(self._name)

        self._proc.configure(self._cfg_file, color)

        # fetch batch filler info
        self._storage = {}
        for i in range(len(self._proc.batch_fillers())):
            pid = self._proc.batch_fillers()[i]
            name = self._proc.storage_name(pid)
            dtype = larcv.BatchDataTypeName(self._proc.batch_types()[i])
            self._storage[name]=batch_pydata(dtype)
            if 'make_copy' in cfg and cfg['make_copy']:
                self._storage[name]._make_copy = True

        # all success?
        # register *this* instance
        self.__class__._instance_m[self._name] = self

    def set_next_batch(self, batch_indexes):
        # if type(batch_indexes) != larcv.VectorOfSizet:
        #     indexes = larcv.VectorOfSizet()
        #     indexes.resize(len(batch_indexes))
        #     for i, val in enumerate(batch_indexes):
        #         indexes[i] = int(val)
        #     batch_indexes = indexes
        self._proc.set_next_batch(batch_indexes)

    def batch_process(self):
        while self.is_reading():
            time.sleep(0.01)
        self._proc.batch_process()


    def prepare_next(self):
        self._proc.prepare_next()

    def is_reading(self,storage_id=None):

        ready = True


        # if any storage items are not ready, it's still reading
        for name,storage in self._storage.items():
            dtype = storage.dtype()
            if dtype == "float32":
                factory = larcv.BatchDataQueueFactoryFloat.get()
            elif dtype == "float64":
                factory = larcv.BatchDataQueueFactoryDouble.get()
            elif dtype == "int":
                factory = larcv.BatchDataQueueFactoryInt.get()
            # These here below are NOT yet wrapped with swig.  Submit a ticket if you need them!
            # elif dtype == "char":
            #    factory = larcv.BatchDataQueueFactoryDouble.get()
            # elif dtype == "short":
            #    factory = larcv.BatchDataQueueFactoryDouble.get()
            # elif dtype == "string":
            #    factory = larcv.BatchDataQueueFactoryDouble.get()
            else:
                factory = None
            batch_storage = factory.get_queue(name)


            ready = ready and batch_storage.is_next_ready()

        ready =  (ready) and not self._proc.is_reading()

        return not ready

    def pop_current_data(self):
        # Promote the "next" data to current in C++ and release current
        self._proc.pop_current_data()

    def next(self,store_entries=False,store_event_ids=False):

        # Calling next will load the next set of data into batch_pydata.  It does not do any
        # automatic data loading or steping, you must do this manually.

        for name,storage in self._storage.items():
            dtype = storage.dtype()
            if dtype == "float32":
                factory = larcv.BatchDataQueueFactoryFloat.get()
            elif dtype == "float64":
                factory = larcv.BatchDataQueueFactoryDouble.get()
            elif dtype == "int":
                factory = larcv.BatchDataQueueFactoryInt.get()
            # These here below are NOT yet wrapped with swig.  Submit a ticket if you need them!
            # elif dtype == "char":
            #    factory = larcv.BatchDataQueueFactoryDouble.get()
            # elif dtype == "short":
            #    factory = larcv.BatchDataQueueFactoryDouble.get()
            # elif dtype == "string":
            #    factory = larcv.BatchDataQueueFactoryDouble.get()
            else:
                factory = None
            batch_storage = factory.get_queue(name)

            batch_data = factory.get_queue(name).get_batch()
            storage.set_data(storage_id=name, larcv_batchdata=batch_data)

        if not store_entries: self._event_entries = None
        else: self._event_entries = self._proc.processed_entries()

        if not store_event_ids: self._event_ids = None
        else: self._event_ids = self._proc.processed_events()

        return

    def fetch_data(self,key):
        try:
            return self._storage[key]
        except KeyError:
            sys.stderr.write('Cannot fetch data w/ key %s (unknown)\n' % key)
            return

    def fetch_event_ids(self):
        return self._event_ids

    def fetch_entries(self):
        return self._event_entries

    def fetch_n_entries(self):
        return self._proc.get_n_entries()
