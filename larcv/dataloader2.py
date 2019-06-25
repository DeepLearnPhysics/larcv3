from . import larcv
import sys,time,os,signal
import numpy as np

class batch_pydata(object):

   _storage_id = -1
   _dtype      = None
   _npy_data   = None
   _dim_data   = None
   _time_copy  = 0
   _time_reshape = 0
   _make_copy  = False

   def __init__(self,dtype):
      self._storage_id = -1
      self._dtype = dtype
      self._npy_data = None
      self._dim_data = None
      self._time_copy = None
      self._time_reshape = None
      
   def batch_data_size(self):
      dsize=1
      for v in self._dim_data: dsize *= v
      return dsize

   def dtype(self): return self._dtype
   def data(self): return self._npy_data
   def dim(self):  return self._dim_data
   def time_copy(self): return self._time_copy
   def time_reshape(self): return self._time_reshape


   def set_data(self,storage_id,larcv_batchdata):
      self._storage_id = storage_id
      dim = larcv_batchdata.dim()

      # set dimension
      if self._dim_data is None:
         self._dim_data = np.array([dim[i] for i in range(len(dim))]).astype(np.int32)

      else:
         if not len(self._dim_data) == len(dim):
            sys.stderr.write('Dimension array length changed (%d => %d)\n' % (len(self._dim_data),len(dim)))
            raise TypeError
         for i in range(len(self._dim_data)):
            if not self._dim_data[i] == dim[i]:
               sys.stderr.write('%d-th dimension changed (%d => %d)\n' % (i,self._dim_data[i],dim[i]))
               raise ValueError

      # copy data into numpy array
      ctime = time.time()
      if self._make_copy:
         if self._npy_data is None:
            # self._npy_data = np.array(larcv_batchdata.data())
            # print("Array type: ", self._npy_data.dtype)
            # Create an array to hold the data if it does not exits:
            self._npy_data = np.ndarray(shape=(larcv_batchdata.data_size()), dtype=self._dtype)
         self._npy_data = self._npy_data.reshape(self.batch_data_size())
         if self._dtype == "float32":
            larcv.copy_array_float(self._npy_data,larcv_batchdata.data())
         elif self._dtype == "float64":
            larcv.copy_array_double(self._npy_data,larcv_batchdata.data())
      else:
         if self._dtype == "int":
            self._npy_data = larcv.as_ndarray_int(larcv_batchdata.data())
         elif self._dtype == "uint":
            self._npy_data = larcv.as_ndarray_uint(larcv_batchdata.data())
         elif self._dtype == "float32":
            self._npy_data = larcv.as_ndarray_float(larcv_batchdata.data())
         elif self._dtype == "float64":
            self._npy_data = larcv.as_ndarray_double(larcv_batchdata.data())
      self._time_copy = time.time() - ctime


      ctime = time.time()
      self._npy_data = self._npy_data.reshape(self._dim_data[0], int(self.batch_data_size()/self._dim_data[0]))
      self.time_data_conv = time.time() - ctime

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

   def configure(self,cfg):
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

      self._proc.configure(self._cfg_file)

      # Set the start entry
      #if self._start_entry is not None:
      #   self._proc.set_start_entry(self._start_entry)

      #if self._entry_skip is not None:
      #   self._proc.set_entries_skip(self._entry_skip)


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

   #def set_start_entry(self, start_entry):
   #   self._start_entry = start_entry

   #def set_entry_skip(self, entry_skip):
   #   self._entry_skip = entry_skip

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

   #def manager_started(self):
   #  if not self._proc or not self._proc.configured():
   #      sys.stderr.write('must call configure(cfg) before manager_started()!\n')
   #  return self._proc.manager_started()

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



