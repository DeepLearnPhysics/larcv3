from . import larcv
import sys,time,os,signal
import numpy as np
from .batch_pydata import batch_pydata
from .larcv_writer import larcv_writer


class thread_interface(object):
    '''
    This class is just a wrapper around the dataloaders and threadio.  You should not feel
    pressured to use it, the threadio python classes work perfectly well.  However, 
    this class implements the same interface as the distributed_larcv_interface class
    which wraps larcv threadio in mpi4py to allow distributed training. See that class for
    more details.

    The bottom line is, if you want to use distributed_larcv_interface, you can use this class
    for a nearly drop-in replacement in prototyping in environments without  MPI, just on a single
    node.
    '''

    def __init__(self,verbose=False):
        '''init function
        
        Not much to store here, just a dict of dataloaders and the keys to access their data.
        '''
        object.__init__(self)
        self._dataloaders = {}
        self._data_keys   = {}
        self._dims        = {}
        self._verbose     = verbose
        self._writer      = None


    # def __del__(self):
    #     # Make sure to stop readers when going out of scope.
    #     self.stop()

    def prepare_writer(self, io_config, output_file=None):

        if self._writer is not None:
            raise Exception("thread_interface doesn't yet support multiple writers.")

        # This only supports batch datasize of 1.  We can check that the reading instance
        # Has only size 1 by looking at the dims.

        key =list(self._data_keys['primary'].items())[0][0]
        if self._dims['primary'][key][0] != 1:
            raise Exception("To use the writing interface, please set batch size to 1.")

        # The writer is not an instance of threadIO but rather an instance of larcv_writer.

        # It configures a process to copy input to output and add more information as well.

        self._writer = larcv_writer(io_config, output_file)

        pass


    def prepare_manager(self, mode, io_config, minibatch_size, data_keys):
        '''Prepare a manager for io
        
        Creates an instance of larcv_threadio for a particular file to read.
        
        Arguments:
            mode {str} -- The mode of training to store this threadio under (typically "train" or "TEST" or similar)
            io_config {dict} -- the io config dictionary.  Required keys are: 'filler_name', 'verbosity', and 'filler_cfg'
            data_keys_override {dict} -- If desired, you can override the keys for dataacces,
        
        Raises:
            Exception -- [description]
        '''


        if mode in self._dataloaders:
            raise Exception("Can not prepare manager for mode {}, already exists".format(mode))

        # Check that the required keys are in the io config:
        for req in ['filler_name', 'verbosity', 'filler_cfg']:
            if req not in io_config:
                raise Exception("io_config for mode {} is missing required key {}".format(mode, req))


        start = time.time()

        # Initialize and configure a manager:
        io = larcv_threadio()
        io.configure(io_config)
        io.start_manager(minibatch_size)
        self._dataloaders.update({mode : io})
        # Force storing here, since it's not configurable on the first read.
        self.next(mode)

        while io.is_reading():
            time.sleep(0.01)

        # Save the manager

        # Store the keys for accessing this datatype:
        self._data_keys[mode] = data_keys
    
        # Read and save the dimensions of the data:
        self._dims[mode] = {}
        for key in self._data_keys[mode]:
            self._dims[mode][key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).dim()

        end = time.time()

        # Print out how long it took to start IO:
        if self._verbose:
            sys.stdout.write("Time to start {0} IO: {1:.2}s\n".format(mode, end - start))


        return


    def next(self, mode):
        self._dataloaders[mode].next(store_event_ids=True, store_entries=True)


    def fetch_minibatch_data(self, mode, fetch_meta_data=False):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._dataloaders['train'].fetch_data(keyword_label).dim() as an example
        
        
        while self._dataloaders[mode].is_reading():
            time.sleep(0.01)

        this_data = {}
        for key in self._data_keys[mode]:
            this_data[key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).data()
            this_data[key] = np.reshape(this_data[key], self._dims[mode][key])

        if fetch_meta_data:
            this_data['entries'] = self._dataloaders[mode].fetch_entries()
            this_data['event_ids'] = self._dataloaders[mode].fetch_event_ids()
        

        return this_data

    def fetch_minibatch_dims(self, mode):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._dataloaders['train'].fetch_data(keyword_label).dim() as an example
        return self._dims[mode]

    # def is_active(self):
    #     _is_active = False
    #     for mode in self._dataloaders:
    #         _is_active = _is_active and self._dataloaders[mode].

    def stop(self):

        for mode in self._dataloaders:
            while self._dataloaders[mode].is_reading():
                time.sleep(0.01)
            self._dataloaders[mode].stop_manager()

        if self._writer is not None:
            self._writer.finalize()

    def size(self, mode):
        # return the number of images in the specified mode:
        return self._dataloaders[mode].fetch_n_entries()

    def ready(self, mode):
        return self._dataloaders[mode].ready()


    def write_output(self, data, datatype, producer, entries, event_ids):
        if self._writer is None:
            raise Exception("Trying to write data with no writer configured.  Abort!")


        self._writer.write(data=data, datatype=datatype, producer=producer, entries=entries, event_ids=event_ids)

        return


class larcv_threadio (object):

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
      self._current_storage_id = -1
      self._current_storage_state = -1
      self._storage = {}
      self._tree_entries = None
      self._event_ids = None
      self._start_entry = None
      self._entry_skip = None

   def reset(self):
      if self._proc: self._proc.reset()

   def __del__(self):
      try:
         self.reset()
      except AttrbuteError:
         pass

   def configure(self,cfg,color=0):
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

      # get ThreadProcessor config file
      self._cfg_file = cfg['filler_cfg']
      if not self._cfg_file or not os.path.isfile(self._cfg_file):
         sys.stderr.write('filler_cfg file does not exist: %s\n' % self._cfg_file)
         raise ValueError

      # set verbosity
      if 'verbosity' in cfg:
         self._verbose = bool(cfg['verbosity'])

      # configure thread processor
      self._proc = larcv.ThreadProcessor(self._name)

      # We have to pass a color here.  Otherwise, if we use this and larcv is built with mpi, it will hang
      self._proc.configure(self._cfg_file)

      # Set the start entry
      if self._start_entry is not None:
         self._proc.set_start_entry(self._start_entry)

      if self._entry_skip is not None:
         self._proc.set_entries_skip(self._entry_skip)


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

   def set_start_entry(self, start_entry):
      self._start_entry = start_entry

   def set_entry_skip(self, entry_skip):
      self._entry_skip = entry_skip

   def start_manager(self, batch_size):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before start_manager()!\n')
         return
      try:
         batch_size=int(batch_size)
         if batch_size<1:
            sys.stderr.write('batch_size must be positive integer!\n')
            raise ValueError
      except TypeError:
         sys.stderr.write('batch_size value/type error. aborting...\n')
         return
      except ValueError:
         sys.stderr.write('batch_size value/type error. aborting...\n')
         return

      self._batch=batch_size
      self._proc.start_manager(batch_size)
      self._current_storage_id=-1
      self._current_storage_state = -1

      while not self._proc.manager_started():
         time.sleep(0.01)

   def stop_manager(self):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before stop_manager()!\n')
         return

      self._batch=None
      self._proc.stop_manager()

   def purge_storage(self):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before purge_storage()!\n')
         return
      self.stop_manager()
      self._proc.release_data()
      self._current_storage_id=-1
      self._current_storage_state = -1
      self._tree_entries = None
      self._event_ids = None

   def manager_started(self):
     if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before manager_started()!\n')
     return self._proc.manager_started()

   def set_next_index(self,index):
      if not self._proc or not self._proc.configured():
         sys.stderr.write('must call configure(cfg) before set_next_index()!\n')
         return
      self._proc.set_next_index(index)

   def is_reading(self,storage_id=None):
      status_v = self._proc.storage_status_array()
      if storage_id is None:
         storage_id = self._current_storage_id+1
         if storage_id >= status_v.size():
            storage_id = 0
      return (not status_v[storage_id] == 3)

   def release(self):
      if not self._current_storage_state == 1:
         return
      self._proc.release_data(self._current_storage_id)
      self._current_storage_state = 0

   def ready(self):
      return self._current_storage_state == 1

   def next(self,store_entries=False,store_event_ids=False):
      if not self._proc or not self._proc.manager_started():
         sys.stderr.write('must call start_manager(batch_size) before next()!\n')
         return

      self.release()


      self._read_start_time = time.time()
      sleep_ctr=0
      next_storage_id = self._current_storage_id + 1
      if next_storage_id == self._proc.num_batch_storage():
         next_storage_id = 0

      while self.is_reading(next_storage_id):
         time.sleep(0.005)
         #sleep_ctr+=1
         #if sleep_ctr%1000 ==0:
         #   print 'queueing storage %d ... (%f sec)' % (next_storage_id,0.05*sleep_ctr)
      self._read_end_time = time.time()

      for name,storage in self._storage.items():
         dtype = storage.dtype()
         if dtype == "float32":
            factory = larcv.BatchDataStorageFactoryFloat.get()
         elif dtype == "float64":
            factory = larcv.BatchDataStorageFactoryDouble.get()
         elif dtype == "int":
            factory = larcv.BatchDataStorageFactoryInt.get()
         # These here below are NOT yet wrapped with swig.  Submit a ticket if you need them!
         # elif dtype == "char":
         #    factory = larcv.BatchDataStorageFactoryDouble.get()
         # elif dtype == "short":
         #    factory = larcv.BatchDataStorageFactoryDouble.get()
         # elif dtype == "string":
         #    factory = larcv.BatchDataStorageFactoryDouble.get()
         else:
            factory = None
         batch_storage = factory.get_storage(name)
         while not batch_storage.is_batch_ready(next_storage_id):
            time.sleep(0.01)
         batch_data = factory.get_storage(name).get_batch(next_storage_id)
         storage.set_data(next_storage_id, batch_data)

      if not store_entries: self._tree_entries = None
      else: self._tree_entries = larcv.VectorOfSizet(self._proc.processed_entries(next_storage_id))

      if not store_event_ids: self._event_ids = None
      else: self._event_ids = larcv.VectorOfEventID(self._proc.processed_events(next_storage_id))

      self._current_storage_id = next_storage_id
      self._current_storage_state = 1
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
      return self._tree_entries

   def fetch_n_entries(self):
      return self._proc.get_n_entries()

def sig_kill(signal,frame):
   print('\033[95mSIGINT detected.\033[00m Finishing the program gracefully.')
   for name,ptr in larcv_threadio._instance_m.items():
      print('Terminating filler: %s' % name)
      ptr.reset()

signal.signal(signal.SIGINT,  sig_kill)



