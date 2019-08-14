import sys
import os
import time
import copy
from collections import OrderedDict
import socket, zlib

import numpy
from mpi4py import MPI

from . threadloader   import larcv_threadio
from . larcv_io_enums import ReadOption

class thread_interface(object):

    def __init__(self, 
                verbose             = False, 
                root                = 0, 
                comm                = MPI.COMM_WORLD, 
                distribute_to_root  = True, 
                read_option         = None, 
                local_rank          = None, 
                local_size          = None,
                backend             = "queue"):

        object.__init__(self)

        if read_option is None:
            self._read_option = ReadOption['read_from_single_rank']
        else:
            self._read_option = ReadOption[read_option]

        # MPI parameters:
        self._comm = comm
        self._size = comm.Get_size()
        self._rank = comm.Get_rank()
        self._root = root

        # MPI local parameters, only needed if reading with option read_from_single_local_rank
        self._local_rank = local_rank
        self._local_size = local_size
        self._local_comm = None
        self._n_groups = None
        self._group_nr = None

        if (self._local_rank is None) and (self._read_option is ReadOption['read_from_single_local_rank']):
            print ('You have selected option {} but have not specified the local rank. Please do so.'.format(self._read_option.name))
            raise Exception("Please specify local_rank in larcv_interface constructor.")

        if (self._local_size is None) and (self._read_option is ReadOption['read_from_single_local_rank']):
            print ('You have selected option {} but have not specified the local size. Please do so.'.format(self._read_option.name))
            raise Exception("Please specify local_size in larcv_interface constructor.")

        if (self._read_option is ReadOption['read_from_single_local_rank']) and (self._size % self._local_size != 0):
            print ('You have selected option {} but global size is not a multiple of local size.'.format(self._read_option.name))
            raise Exception("Global size is not a multiple of local size..")

        # Need to define group MPI communicators 
        # to communicate among ranks in a single node
        if (self._read_option is ReadOption['read_from_single_local_rank']):
            
            # Get the hostname
            hostname = socket.gethostname().encode('utf-8')

            # Get unique integer (color) from name
            color = zlib.crc32(hostname)
            c_max_int = 2147483647
            while (color >= c_max_int):
                color -= c_max_int

            # Split the world comm into local comm based on color
            local_comm = comm.Split(color, self._rank)

            # Save the local comm
            self._local_comm = local_comm
            
            # Find out how many local comm we have
            self._n_groups = int(self._size / local_size)
            self._group_nr = int(self._rank / local_size)


        # This option controls whether or not to distrubute data to the root process
        self._distribute_to_root = distribute_to_root

        # Parameters based from the non MPI version:
        self._dims        = {}
        self._dataloaders = {}
        self._data_keys   = {}
        self._verbose     = verbose
        
        # These are used to correctly scatterv the data:
        self._counts      = {}
        self._displs      = {}
        self._dtypes      = {}

        # These ONLY get populated on the root node:
        self._raw_dims    = {}
        self._raw_dtypes  = {}

        self._datasize    = {}

        return

    def prepare_manager(self, mode, io_config, minibatch_size, data_keys):
        '''
        This has to be called on all ranks, not just the root rank
        '''

        # If we're on the root rank, we can prepare the interface and broadcast the dimensions.
        # Otherwise, just receive the dimensions:

        # Store the keys for accessing this datatype:
        # (This gets passed on every rank)

        if type(data_keys) is not OrderedDict:
            raise Exception("Please pass the data keys via OrderedDict, to ensure mpi messaging order is well defined.")

        self._data_keys[mode] = data_keys

        if ((self._rank == self._root and self._read_option is ReadOption['read_from_single_rank']) 
          or (self._read_option is ReadOption['read_from_all_ranks_copy'])
          or (self._local_rank == self._root and self._read_option is ReadOption['read_from_single_local_rank'])):
        
            if mode in self._dataloaders:
                raise Exception("Can not prepare manager for mode {}, already exists".format(mode))

            # Check that the required keys are in the io config:
            for req in ['filler_name', 'verbosity', 'filler_cfg']:
                if req not in io_config:
                    raise Exception("io_config for mode {} is missing required key {}".format(mode, req))

            print ('Initializing rank', self._rank, 'with mode', mode, '- it will read', minibatch_size, 'entries.')

            start = time.time()

            # Initialize and configure a manager:
            io = larcv_threadio()

            if (self._read_option is ReadOption['read_from_all_ranks_copy']):
                io.set_start_entry(self._rank * minibatch_size)
                io.set_entry_skip(self._size * minibatch_size)
            if (self._read_option is ReadOption['read_from_single_local_rank']):
                io.set_start_entry(self._group_nr * minibatch_size)
                io.set_entry_skip(self._n_groups * minibatch_size)

            io.configure(io_config)
            io.start_manager(minibatch_size)

            # Wait for manager to start
            while (not io.manager_started()):
                print ('Waiting for manager to start for mode {}...'.format(mode))
                time.wait(0.05)

            # Save the manager
            self._dataloaders.update({mode : io})
            self._dataloaders[mode].next()

    
            # Read and save the dimensions of the data:
            # As the root processes (for IO), set the dims and then distribute to everyone:
            self._raw_dims[mode]   = {}
            self._raw_dtypes[mode] = {}
            for key in self._data_keys[mode]:
                self._raw_dims[mode][key]   = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).dim()
                self._raw_dtypes[mode][key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).data().dtype

            end = time.time()

            # Print out how long it took to start IO:
            if self._verbose:
                sys.stdout.write("Time to start {0} IO: {1:.2}s\n".format(mode, end - start))

        # call set dims to distribute the dimensions to everyone:
        self._set_dims(mode)

        return


    def _set_dims(self, mode):
        ''' 
        This function is used to broadcast the shape and dimensions 
        of the data used.  It is called automatically from prepare manager,
        do not call it yourself.
        '''

        if (self._read_option is ReadOption['read_from_all_ranks_copy']):
            self._dims[mode] = self._raw_dims[mode]
            self._datasize[mode] = self._dataloaders[mode].fetch_n_entries()
            self._dtypes[mode] = self._raw_dtypes[mode]

            self._counts[mode] = {}
            self._displs[mode] = {}

        elif (self._read_option is ReadOption['read_from_single_local_rank']):

            if self._local_rank != self._root:
                dims = None
            else:
                # the number of events per worker needs to evenly divide the number
                # of batches for this to succeed.  So, check and adjust the dims:
                first_key = next(iter(self._data_keys[mode]))
                n_raw_batch_size = self._raw_dims[mode][first_key][0]
                n_worker_nodes = self._local_size 
                if n_raw_batch_size % n_worker_nodes != 0:
                    print ("Requested to broadcast {} images to {} workers, please adjust to distribute evenly".format(
                        n_raw_batch_size, n_worker_nodes))
                    raise Exception("Requested to broadcast {} images to {} workers, please adjust to distribute evenly".format(
                        n_raw_batch_size, n_worker_nodes))
                # Make a copy of the raw dimensions so we can scale it down for broadcasting:
                dims = copy.deepcopy(self._raw_dims[mode])
                for key in self._data_keys[mode]:
                    dims[key][0] /= n_worker_nodes

            # Actually broadcast the dimensions:
            dims = self._comm.bcast(dims, root=self._root)
            # Note: this is relying on pickling the dimensions, which is a dict of numpy arrays
            # It's slow, yes, but the data is so small that it shoul not be a big overhead.

            # And store them:
            self._dims[mode] = dims
            
            # Broadcast the total size of the dataset:
            if self._local_rank == self._root:
                datasize = self._dataloaders[mode].fetch_n_entries()
            else:
                datasize = 0

            self._datasize[mode] = self._comm.bcast(datasize, root = self._root)

            # Distribute the counts and displacements necessary for scatterv
            self._counts[mode] = {}
            self._displs[mode] = {}

            # Lastly, distribute the dtypes necessary for scatterv to work properly:
            if self._local_rank != self._root:
                dtypes = None
            else:
                dtypes = self._raw_dtypes[mode]

            self._dtypes[mode]= self._comm.bcast(dtypes, root=self._root)

        else:

            if self._rank != self._root:
                dims = None
            else:
                # the number of events per worker needs to evenly divide the number
                # of batches for this to succeed.  So, check and adjust the dims:
                first_key = next(iter(self._data_keys[mode]))
                n_raw_batch_size = self._raw_dims[mode][first_key][0]

                # How many worker nodes depends on whether or not we're distributing to the root node:
                n_worker_nodes = self._size if self._distribute_to_root else self._size - 1

                if n_raw_batch_size % n_worker_nodes != 0:
                    raise Exception("Requested to broadcast {} images to {} workers, please adjust to distribute evenly".format(
                        n_raw_batch_size, n_worker_nodes))
            
                # Make a copy of the raw dimensions so we can scale it down for broadcasting:
                dims = copy.deepcopy(self._raw_dims[mode])
                for key in self._data_keys[mode]:
                    dims[key][0] /= n_worker_nodes

            # Actually broadcast the dimensions:
            dims = self._comm.bcast(dims, root=self._root)
            # Note: this is relying on pickling the dimensions, which is a dict of numpy arrays
            # It's slow, yes, but the data is so small that it shoul not be a big overhead.

            # And store them:
            self._dims[mode] = dims
            
            # Broadcast the total size of the dataset:
            if self._rank == self._root:
                datasize = self._dataloaders[mode].fetch_n_entries()
            else:
                datasize = 0

            self._datasize[mode] = self._comm.bcast(datasize, root = self._root)

            # Distribute the counts and displacements necessary for scatterv
            self._counts[mode] = {}
            self._displs[mode] = {}

            # Lastly, distribute the dtypes necessary for scatterv to work properly:
            if self._rank != self._root:
                dtypes = None
            else:
                dtypes = self._raw_dtypes[mode]

            self._dtypes[mode]= self._comm.bcast(dtypes, root=self._root)



        # Calculate the total size of each message recieved by each node, 
        # And it's displacement from the start of the total message

        this_size = self._size
        if self._read_option is ReadOption['read_from_single_local_rank']:
            this_size = self._local_size

        for key in self._data_keys[mode]:
            size = int(numpy.prod(self._dims[mode][key]))
            # Size of the message is just the produce of the worker batch dims
            self._counts[mode][key] = numpy.ones(this_size, numpy.int) * size
            
            # If not distributing to root, the root node gets nothing:
            if not self._distribute_to_root:
                self._counts[mode][key][self._root] = 0

            # The displacements is just the sum of the counts, but started from 0
            self._displs[mode][key] = numpy.cumsum(self._counts[mode][key]) - size
        
        return 


    def read_and_distribute_data(self, mode, unscattered_data=None):
        '''
        Read the next batch of data from the interface (which, presumably, is 
        threaded so the data is ready to go when this is called) and distribute it
        to the MPI Comm specified (Default = MPI.COMM_WORLD)
        '''

        # Use the world communicator and global rank if scattering to all ranks
        comm = self._comm
        rank = self._rank

        # Find the right communicator for this group and use the local rank if reading from every node
        if self._read_option is ReadOption['read_from_single_local_rank']:
            rank = self._local_rank
            comm = self._local_comm

        # We have the private data only on the root rank (or local rank):
        private_data = {}

        for key in self._data_keys[mode]:

            if key not in unscattered_data:
                continue

            # Create a buffer for the data to scatter
            # Which is just a reference to the numpy array
            sendbuff = None
            if rank == self._root:
                sendbuff = unscattered_data[key]
                
            # The recvbuff must be properly sized:
            recvbuff = numpy.empty((self._counts[mode][key][rank]), dtype=self._dtypes[mode][key])

            # Scatterv will scatter numpy arrays, and note the automatic lookup of 
            # dtypes from numpy to MPI.  If you are getting crazy, undefined or NaN values,
            # the dtype is a good start for investigating.
            comm.Scatterv([ sendbuff,
                            self._counts[mode][key], 
                            self._displs[mode][key],
                            MPI._typedict[self._dtypes[mode][key].char]],
                            recvbuff,
                            root=self._root)

            #print (self._rank, self._local_rank, 'done Scatterv!')

            # Reshape the data (all ranks except root, unless distributing to root)
            if self._distribute_to_root or rank != self._root:                
                private_data[key] = numpy.reshape(recvbuff, self._dims[mode][key])
            else:
                private_data[key] = None

        return private_data


    def finish(self):
        rank = self._rank
        if self._read_option is ReadOption['read_from_single_local_rank']:
            rank = self._local_rank

        if rank == self._root:
            for mode in self._dataloaders:
                self._dataloaders[mode].stop_manager()


    def fetch_minibatch_data(self, mode, fetch_meta_data=False):
        '''
        Read data from disk and distribute it to all other workers.
        '''

        # sys.stdout.write("Rank {} starting to fetch minibatch data\n".format(self._rank))

        # If this is the root node, read the data from disk:
        if ((self._rank == self._root and self._read_option is ReadOption['read_from_single_rank']) 
          or (self._read_option is ReadOption['read_from_all_ranks_copy'])
          or (self._local_rank == self._root and self._read_option is ReadOption['read_from_single_local_rank'])):
            unscattered_data = {}
            for key in self._data_keys[mode]:
                unscattered_data[key] = self._dataloaders[mode].fetch_data(self._data_keys[mode][key]).data()
        else:
            unscattered_data = {}
            for key in self._data_keys[mode]:
                unscattered_data[key]  = None

        this_data = {}
        if (self._read_option is not ReadOption['read_from_all_ranks_copy']):
            this_data = self.read_and_distribute_data(mode, unscattered_data)
        else:
            for key in self._data_keys[mode]:
                if key not in unscattered_data: continue
                this_data[key] = numpy.reshape(unscattered_data[key], self._dims[mode][key])

        if ((self._rank == self._root and self._read_option is ReadOption['read_from_single_rank'])
          or (self._read_option is ReadOption['read_from_all_ranks_copy'])
          or (self._local_rank == self._root and self._read_option is ReadOption['read_from_single_local_rank'])):
            self._dataloaders[mode].next()

        return this_data

    def fetch_minibatch_dims(self, mode):
        # Return a dictionary object with keys 'image', 'label', and others as needed
        # self._dataloaders['train'].fetch_data(keyword_label).dim() as an example
        return self._dims[mode]

    def stop(self):
        if self._rank == self._root:
            for mode in self._dataloaders:
                while self._dataloaders[mode].is_reading():
                    time.sleep(0.01)
                self._dataloaders[mode].stop_manager()


    def size(self, mode):
        return self._datasize[mode]
