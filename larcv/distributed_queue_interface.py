import sys
import os
import time
import copy
from collections import OrderedDict
import socket, zlib
import random

import numpy
from mpi4py import MPI

from larcv            import larcv
from . queueloader    import larcv_queueio
from . larcv_io_enums import ReadOption, RandomAccess

class queue_interface(object):

    def __init__(self, 
            verbose             = False, 
            random_access_mode  = "random_blocks",
            entry_comm          = MPI.COMM_WORLD,
            io_comm             = MPI.COMM_WORLD):

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

        self._entry_comm = entry_comm
        self._io_comm    = io_comm

        self._queue_prev_entries = {}
        self._queue_next_entries = {}

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
            next_entries = numpy.arange(minibatch_size, dtype=numpy.int32) + last_entry + 1

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
        
        next_entries = numpy.asarray(next_entries, dtype=numpy.int32)
        self._queue_next_entries[mode] = next_entries


        return next_entries

    def coordinate_next_batch_indexes(self, mode, comm, root_rank = 0):
        ''' 
        This function is a little naieve (sp?).  But it will take the root rank, 
        use it to determine the next batch indexes, then scatter those indexes.

        It will scatter it evenly to the entire comm that is passed in.
        '''

        comm_size = comm.Get_size()

        if comm.Get_rank() == root_rank:
            set_entries = self.get_next_batch_indexes(mode, self._minibatch_size[mode])


        # Create a buffer for the data to scatter
        # Which is just a reference to the numpy array
        sendbuff = None
        if comm.Get_rank() == root_rank:
            sendbuff = set_entries
            
        local_size = int(self._minibatch_size[mode] / comm_size)

        # The recvbuff must be properly sized:
        recvbuff = numpy.empty((local_size), dtype=numpy.int32)

        # Scatterv will scatter numpy arrays, and note the automatic lookup of 
        # dtypes from numpy to MPI.  If you are getting crazy, undefined or NaN values,
        # the dtype is a good start for investigating.
        comm.Scatter(sendbuff,
                     recvbuff,
                     root=root_rank)

        #print (self._rank, self._local_rank, 'done Scatterv!')



        return recvbuff


    def prepare_manager(self, mode, io_config, minibatch_size, data_keys, color):
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
        io.configure(io_config, color)
        self._queueloaders.update({mode : io})
        self._minibatch_size[mode] = minibatch_size

        # Queue loaders are manually triggered IO, not always running, so
        # there is no "start_manager" function.  Everything is manual.
        # First, tell it what the entries for the first batch to read:
        set_entries = self.coordinate_next_batch_indexes(mode, comm=self._entry_comm)


        io.set_next_batch(set_entries)
        # This will wait to return until things have processed:
        io.batch_process()

        # Then, we promote those entries to the "current" batch:
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

        # Print out how long it took to start IO:
        if self._verbose:
            sys.stdout.write("Time to start {0} IO: {1:.2}s\n".format(mode, end - start))


        return

    def prepare_next(self, mode, set_entries = None):
        '''Set in motion the processing of the next batch of data.
        
        Triggers the queue loader to start reading the next set of data
        

        '''
        # Which events should we read?
        if set_entries is None:
            set_entries = self.coordinate_next_batch_indexes(mode, comm=self._entry_comm)
            # set_entries = self.get_next_batch_indexes(mode, self._minibatch_size[mode])
            
        self._queueloaders[mode].set_next_batch(set_entries)
        self._queueloaders[mode].batch_process()
        
        # t.daemon = True
        return 
        # return threading.Thread(target=self._queueloaders[mode].batch_process).start()

        # self._queueloaders[mode].next(store_event_ids=True, store_entries=True)

        # return

    def fetch_minibatch_data(self, mode, pop=False, fetch_meta_data=False):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._queueloaders['train'].fetch_data(keyword_label).dim() as an example
        
        if pop:
            # This function will pop the data
            while self._queueloaders[mode].is_reading():
                time.sleep(0.001)
            self._queueloaders[mode].pop_current_data()

        self._queueloaders[mode].next(store_entries=fetch_meta_data, store_event_ids=fetch_meta_data)
        this_data = {}
        
        for key in self._data_keys[mode]:
            this_data[key] = self._queueloaders[mode].fetch_data(self._data_keys[mode][key]).data()
            this_data[key] = numpy.reshape(this_data[key], self._dims[mode][key])

        if fetch_meta_data:
            this_data['entries'] = self._queueloaders[mode].fetch_entries()
            this_data['event_ids'] = self._queueloaders[mode].fetch_event_ids()
        

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

        for mode in self._queueloaders:
            while self._queueloaders[mode].is_reading():
                time.sleep(0.01)
            self._queueloaders[mode].stop_manager()

        if self._writer is not None:
            self._writer.finalize()

    def size(self, mode):
        # return the number of images in the specified mode:
        return self._queueloaders[mode].fetch_n_entries()


    def ready(self, mode):
        return self._queueloaders[mode].ready()

