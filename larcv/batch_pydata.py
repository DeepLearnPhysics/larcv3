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
         self._npy_data = np.copy(larcv_batchdata.pydata())

      else:
         self._npy_data = larcv_batchdata.pydata()

      self._time_copy = time.time() - ctime


      ctime = time.time()
      self._npy_data = self._npy_data.reshape(self._dim_data[0], int(self.batch_data_size()/self._dim_data[0]))
      self.time_data_conv = time.time() - ctime

      return