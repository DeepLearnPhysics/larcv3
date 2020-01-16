from . import larcv
import sys,time,os,signal
import numpy as np

class batch_pydata(object):

   _storage_id   = -1
   _dtype        = None
   _npy_data     = None
   _dim_data     = None
   _dim_dense    = None
   _time_copy    = 0
   _time_reshape = 0
   _make_copy    = False

   def __init__(self,dtype):
      self._storage_id   = -1
      self._dtype        = dtype
      self._npy_data     = None
      self._dim_data     = None
      self._dim_dense    = None
      self._time_copy    = None
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
      dim   = larcv_batchdata.dim()
      dense = larcv_batchdata.dense_dim()



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

      # Set dense dimension:
      if self._dim_dense is None:
         self._dim_dense = np.array([dense[i] for i in range(len(dense))]).astype(np.int32)
      else:
         if not len(self._dim_dense) == len(dense):
            sys.stderr.write('Dimension array length changed (%d => %d)\n' % (len(self._dim_dense),len(dense)))
            raise TypeError
         for i in range(len(self._dim_dense)):
            if not self._dim_dense[i] == dense[i]:
               sys.stderr.write('%d-th dimension changed (%d => %d)\n' % (i,self._dim_dense[i],dense[i]))
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

   def as_dense():

      '''
      This format converts the larcv sparse format to
      the tuple format required for sparseconvnet
      '''

      # If the data is from BatchFillerSparseTensor2D,
      # it will have the shape (batch_size, n_planes, n_elements_per_plane, 2 or 3)
      # where the 2 or 3 is if values are included or not.

      # If the data is from BatchFillerSparseTensor3D, it will have the
      # shape (batchsize, n_elements, 3 or 4) where the 3 or 4 
      # is if values are included or not

      if len(self._dim_data) == 4:
         # This is 2D
         return None
      elif len(self._dim_data) == 3:
         # This is 3D
         return None

   def as_torch_geometric():

      '''
      This format converts the larcv sparse format to
      the tuple format required for sparseconvnet
      '''

      # If the data is from BatchFillerSparseTensor2D,
      # it will have the shape (batch_size, n_planes, n_elements_per_plane, 2 or 3)
      # where the 2 or 3 is if values are included or not.

      # If the data is from BatchFillerSparseTensor3D, it will have the
      # shape (batchsize, n_elements, 3 or 4) where the 3 or 4 
      # is if values are included or not

      if len(self._dim_data) == 4:
         # This is 2D
         return None
      elif len(self._dim_data) == 3:
         # This is 3D
         return None

   def as_scnsparse():

      '''
      This format converts the larcv sparse format to
      the tuple format required for sparseconvnet
      '''

      # If the data is from BatchFillerSparseTensor2D,
      # it will have the shape (batch_size, n_planes, n_elements_per_plane, 2 or 3)
      # where the 2 or 3 is if values are included or not.

      # If the data is from BatchFillerSparseTensor3D, it will have the
      # shape (batchsize, n_elements, 3 or 4) where the 3 or 4 
      # is if values are included or not

      if len(self._dim_data) == 4:
         # This is 2D
         return None
      elif len(self._dim_data) == 3:
         # This is 3D
         return None

#       # First, we can split off the features (which is the pixel value)
#       # and the indexes (which is everythin else)
#       n_dims = input_array.shape[-1]

#       split_tensors = numpy.split(input_array, n_dims, axis=-1)


#       # To map out the non_zero locations now is easy:
#       non_zero_inds = numpy.where(split_tensors[-1] != -999)

#       # The batch dimension is just the first piece of the non-zero indexes:
#       batch_size  = input_array.shape[0]
#       batch_index = non_zero_inds[0]

#       # Getting the voxel values (features) is also straightforward:
#       features = numpy.expand_dims(split_tensors[-1][non_zero_inds],axis=-1)

#       # Lastly, we need to stack up the coordinates, which we do here:
#       dimension_list = []
#       for i in range(len(split_tensors) - 1):
#         dimension_list.append(split_tensors[i][non_zero_inds])

#       # Tack on the batch index to this list for stacking:
#       dimension_list.append(batch_index)

#       # And stack this into one numpy array:
#       dimension = numpy.stack(dimension_list, axis=-1)

#       output_array = (dimension, features, batch_size,)
#       return output_array




# def larcvsparse_to_scnsparse_3d(input_array):



# def larcvsparse_to_scnsparse_2d(input_array):
#     # This format converts the larcv sparse format to
#     # the tuple format required for sparseconvnet

#     # First, we can split off the features (which is the pixel value)
#     # and the indexes (which is everythin else)

#     # To handle the multiplane networks, we have to split this into
#     # n_planes and pass it out as a list

#     n_planes = input_array.shape[1]
#     batch_size = input_array.shape[0]


#     raw_planes = numpy.split(input_array,n_planes, axis=1)

#     output_list = []
#     output_features = []
#     output_dimension = []

#     for i, plane in enumerate(raw_planes):
#         # First, squeeze off the plane dimension from this image:
#         plane = numpy.squeeze(plane, axis=1)

#         # Next, figure out the x, y, value coordinates:
#         x,y,features = numpy.split(plane, 3, axis=-1)


#         non_zero_locs = numpy.where(features != -999)
#         # Pull together the different dimensions:
#         x = x[non_zero_locs]
#         y = y[non_zero_locs]
#         p = numpy.full(x.shape, fill_value=i)
#         features = features[non_zero_locs]
#         features = numpy.expand_dims(features,axis=-1)

#         batch = non_zero_locs[0]

#         # dimension = numpy.concatenate([x,y,batch], axis=0)
#         # dimension = numpy.stack([x,y,batch], axis=-1)
#         dimension = numpy.stack([p,x,y,batch], axis=-1)

#         output_features.append(features)
#         output_dimension.append(dimension)

#     output_features = numpy.concatenate(output_features)
#     output_dimension = numpy.concatenate(output_dimension)

#     output_list = [output_dimension, output_features, batch_size]

#     return output_list



