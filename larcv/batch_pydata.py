import larcv
import sys,time,os,signal
import numpy 

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

   def dtype(self): 
      return self._dtype

   def data(self, shape=None, channels="last"): 
      '''Return the data of this batch
      
      larcv is generally used to read sparse data and return in, often,
      a sparse format.  This is acheived by packing into a buffer and returning
      directly a buffer.  This function can quickly parse the buffer and expand
      to a dense or other popular sparse shape.  If shape is none, it's untouched.
      
      Keyword Arguments:
         shape {str} -- Set to "dense", "scnsparse", or "torch_geometric" to expand the data (default: {None})
         channels {str} -- If shape is "dense", channels first or last? (default: {"last"})
      
      Returns:
         [type] -- [description]
      '''
      if shape is None:
         return numpy.reshape(self._npy_data, self._dim_data)
      elif shape == "dense":
         return self.as_dense(channels)
      elif shape == "scnsparse":
         return self.as_scnsparse()
      elif shape == "torch_geometric":
         return self.as_torch_geometric()

   def dim(self):  
      return self._dim_data

   def time_copy(self): 
      return self._time_copy

   def time_reshape(self): 
      return self._time_reshape

   def set_data(self,storage_id,larcv_batchdata):
      self._storage_id = storage_id
      dim   = larcv_batchdata.dim()
      dense = larcv_batchdata.dense_dim()



      # set dimension
      # if self._dim_data is None:
      # Dimension may change from shot to shot.  Refresh every time:
      self._dim_data  = numpy.array([dim[i] for i in range(len(dim))]).astype(numpy.int32)
      self._dense_dim = numpy.array([dense[i] for i in range(len(dense))]).astype(numpy.int32)


               
      # copy data into numpy array
      ctime = time.time()
      if self._make_copy:
         if self._npy_data is None:
            # self._npy_data = numpy.array(larcv_batchdata.data())
            # print("Array type: ", self._npy_data.dtype)
            # Create an array to hold the data if it does not exits:
            self._npy_data = numpy.ndarray(shape=(larcv_batchdata.data_size()), dtype=self._dtype)
         self._npy_data = self._npy_data.reshape(self.batch_data_size())
         self._npy_data = numpy.copy(larcv_batchdata.pydata())

      else:
         self._npy_data = larcv_batchdata.pydata()



      self._time_copy = time.time() - ctime


      ctime = time.time()
      self._npy_data = numpy.reshape(self._npy_data, self._dim_data)
      # self._npy_data = self._npy_data.reshape(self._dim_data[0], int(self.batch_data_size()/self._dim_data[0]))
      self.time_data_conv = time.time() - ctime

      return

   def as_dense(self, channels="last"):

      '''
      This format converts the larcv sparse format to
      the dense format
      '''

      # If the data is from BatchFillerSparseTensor2D,
      # it will have the shape (batch_size, n_planes, n_elements_per_plane, 2 or 3)
      # where the 2 or 3 is if values are included or not.

      # If the data is from BatchFillerSparseTensor3D, it will have the
      # shape (batchsize, n_elements, 3 or 4) where the 3 or 4 
      # is if values are included or not




      if len(self._dim_data) == 4:
         # This is 2D
         # Initialize an output array.  Larcv puts channels last by default (B, W, H, Ch)
         if channels == "last":
            output_array = numpy.zeros(
               [self._dense_dim[0], self._dense_dim[2], self._dense_dim[1], self._dense_dim[3]],
               dtype=self._dtype)
         else:
            output_array = numpy.zeros(
               [self._dense_dim[0], self._dense_dim[3], self._dense_dim[2], self._dense_dim[1]],
               dtype=self._dtype)

         # Slice the input array into x, y, vals:
         x_coords = self._npy_data[:,:,:,0]
         y_coords = self._npy_data[:,:,:,1]
         val_coords = self._npy_data[:,:,:,2]
         
         # Where are the non-zero locations?
         filled_locs = val_coords != -999
         
         # Map the output locations based on non-zero locations:
         batch_index, plane_index, voxel_index = numpy.where(filled_locs)

         # Pull out only the important values:
         values  = val_coords[batch_index, plane_index, voxel_index]
         x_index = numpy.int32(x_coords[batch_index, plane_index, voxel_index])
         y_index = numpy.int32(y_coords[batch_index, plane_index, voxel_index])
         if channels == "last":
            output_array[batch_index, y_index, x_index, plane_index] = values
         else:
            output_array[batch_index, plane_index, y_index, x_index] = values
         return output_array

      elif len(self._dim_data) == 3:
         # This is 3D
         # Initialize an output array.  Larcv puts channels last by default (B, X, Y, Z, Ch==1)
         if channels == "last":
            output_array = numpy.zeros(
               [self._dense_dim[0], self._dense_dim[1], self._dense_dim[2], self._dense_dim[3], self._dense_dim[4]],
               dtype=self._dtype)
         else:
            output_array = numpy.zeros(
               [self._dense_dim[0], self._dense_dim[4], self._dense_dim[1], self._dense_dim[2], self._dense_dim[3]],
               dtype=self._dtype)
         print(output_array.shape)

         # Slice the input array into x, y, vals:
         x_coords   = self._npy_data[:,:,0]
         y_coords   = self._npy_data[:,:,1]
         z_coords   = self._npy_data[:,:,2]
         val_coords = self._npy_data[:,:,3]
         
         # Where are the non-zero locations?
         filled_locs = val_coords != -999
         
         # Map the output locations based on non-zero locations:
         batch_index, voxel_index = numpy.where(filled_locs)


         values  = val_coords[batch_index, voxel_index]
         x_index = numpy.int32(x_coords[batch_index, voxel_index])
         y_index = numpy.int32(y_coords[batch_index, voxel_index])
         z_index = numpy.int32(z_coords[batch_index, voxel_index])


         # Fill in the output tensor

         output_array[batch_index, 0, x_index, y_index, z_index] = values    

         if channels == "last":
            output_array[batch_index, x_index, y_index, z_index, 0 ] = values
         else:
            output_array[batch_index, 0, x_index, y_index, z_index ] = values
         return output_array
       



   def as_torch_geometric(self):

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

   def as_scnsparse(self):

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



