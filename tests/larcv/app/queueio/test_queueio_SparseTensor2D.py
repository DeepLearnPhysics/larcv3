import pytest
import unittest
import random
import uuid
import numpy
import time

import larcv
from larcv import  data_generator

from collections import OrderedDict



def create_sparsetensor2d_file(file_name, rand_num_events, n_projections):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=2, n_projections=n_projections)

@pytest.mark.parametrize('batch_size', [1, 2])
@pytest.mark.parametrize('n_projections', [1,2])
def test_sparsetensor2d_queueio(tmpdir, batch_size, n_projections, n_reads=10):


    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_sparsetensor2d_{}.h5".format(queueio_name))

    # Next, write some sparsetensor2ds to that file:
    create_sparsetensor2d_file(file_name, rand_num_events=25, n_projections=n_projections)


    # Now, let's get the configuration of a queueio object:
    default_config = larcv.QueueProcessor.default_config()
    default_config["InputFiles"].append(file_name)

    # Add the batch filler to the default config:
    default_config["Verbosity"] = 0
    default_config["ProcessDriver"]["ProcessName"].append(f"test_{queueio_name}")
    default_config["ProcessDriver"]["ProcessType"].append("BatchFillerSparseTensor2D")

    process_list = {f"test_{queueio_name}": 
    {
      "TensorProducer": "test",
      "MaxVoxels": 100,
      "UnfilledVoxelValue": -999,
      "Channels": list(range(n_projections)),
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
    queue_proc.prepare_next()
    while queue_proc.is_reading():
      print("Sleeping")
      time.sleep(0.1)
    queue_proc.pop_current_data()

    for i in range(n_reads):
      batch=indexes[i*batch_size:(i+1)*batch_size]
      queue_proc.set_next_batch(batch)
      queue_proc.prepare_next()
      
      while queue_proc.is_reading():
        print("Sleeping")
        time.sleep(0.1)


if __name__ == "__main__":
    test_sparsetensor2d_queueio("./", batch_size=2, n_projections=1, n_reads=10)
    test_sparsetensor2d_queueio("./", batch_size=2, n_projections=2, n_reads=10)
    print("Success")
