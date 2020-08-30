import pytest
import unittest
import random
import uuid
import numpy
import time

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict




def create_tensor3d_file(file_name, rand_num_events, n_projections=1):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=3, n_projections=n_projections)

def create_dense_tensor3d_file(file_name, rand_num_events, n_projections=1):
    voxel_set_list = data_generator.build_tensor(rand_num_events, dimension=3, n_projections = n_projections)
    data_generator.write_tensor(file_name, voxel_set_list, dimension=3)



@pytest.mark.parametrize('make_copy', [False])
@pytest.mark.parametrize('batch_size', [1,2])
@pytest.mark.parametrize('from_dense', [False, True])
def test_tensor3d_queueio(tmpdir, make_copy, batch_size, from_dense, n_reads=2):


    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_tensor3d_{}.h5".format(queueio_name))

    # Next, write some tensor3ds to that file:
    if from_dense:
      create_dense_tensor3d_file(file_name, rand_num_events=25)
    else:
      create_tensor3d_file(file_name, rand_num_events=25)


    if from_dense:
        tensor_type = "dense"
    else:
        tensor_type = "sparse"

    # Now, let's get the configuration of a queueio object:
    default_config = larcv.QueueProcessor.default_config()
    default_config["InputFiles"].append(file_name)

    # Add the batch filler to the default config:
    default_config["Verbosity"] = 0
    default_config["ProcessDriver"]["ProcessName"].append(f"test_{queueio_name}")
    default_config["ProcessDriver"]["ProcessType"].append("BatchFillerTensor3D")

    process_list = {f"test_{queueio_name}": 
    {
      "TensorProducer": "test",
      "TensorType": tensor_type,
      "Channels": [0,],
      "EmptyVoxelValue": -999.,
      "Augment": True,
      }
    }

    if default_config["ProcessDriver"]["ProcessList"] is None:
      default_config["ProcessDriver"]["ProcessList"] = process_list
    else:
      default_config["ProcessDriver"]["ProcessList"].append(process_list)
 

    queue_proc = larcv.QueueProcessor()
    queue_proc.configure(default_config)


    indexes = numpy.arange(batch_size*n_reads*2) % queue_proc.get_n_entries()

    queue_proc.set_next_batch(indexes[0:batch_size])
    print(queue_proc.is_reading())
    queue_proc.prepare_next()
    time.sleep(2)
    while queue_proc.is_reading():
      print("Sleeping")
      time.sleep(0.5)
    queue_proc.pop_current_data()

    for i in range(n_reads):
      batch=indexes[i*batch_size:(i+1)*batch_size]
      queue_proc.set_next_batch(batch)
      queue_proc.prepare_next()
      
      while queue_proc.is_reading():
        print("Sleeping")
        time.sleep(0.1)



if __name__ == "__main__":
    test_tensor3d_queueio("./", make_copy=False, batch_size=1, from_dense=False, n_reads=2)
    test_tensor3d_queueio("./", make_copy=False, batch_size=1, from_dense=True, n_reads=2)
    print("Success")
