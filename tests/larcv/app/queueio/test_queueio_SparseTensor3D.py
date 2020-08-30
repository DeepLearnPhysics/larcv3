import pytest
import unittest
import random
import uuid
import time
import numpy

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict


def create_sparsetensor3d_file(file_name, rand_num_events, n_projections=1):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=3, n_projections=n_projections)


@pytest.mark.parametrize('batch_size', [1, 2])
def test_sparsetensor3d_queueio(tmpdir, batch_size, n_reads=10):


    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_sparsetensor3d_{}.h5".format(queueio_name))

    # Next, write some sparsetensor3ds to that file:
    create_sparsetensor3d_file(file_name, rand_num_events=25)


    # Now, let's get the configuration of a queueio object:
    default_config = larcv.QueueProcessor.default_config()
    default_config["InputFiles"].append(file_name)

    # Add the batch filler to the default config:
    default_config["Verbosity"] = 0
    default_config["ProcessDriver"]["ProcessName"].append(f"test_{queueio_name}")
    default_config["ProcessDriver"]["ProcessType"].append("BatchFillerSparseTensor3D")

    process_list = {f"test_{queueio_name}": 
    {
      "TensorProducer": "test",
      "MaxVoxels": 100,
      "UnfilledVoxelValue": -999,
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



# @pytest.mark.distributed_test
# @pytest.mark.parametrize('local_batch_size', [2])
# def test_sparsetensor3d_queueio_distributed(tmpdir, local_batch_size, n_reads=10):

#     from larcv import distributed_queue_interface
#     from mpi4py import MPI
#     comm = MPI.COMM_WORLD

#     root_rank = comm.Get_size() - 1
#     comm_size = comm.Get_size()

#     queueio_name = "queueio_{}".format(uuid.uuid4())

#     config_file = tmpdir + "/test_queueio_sparsetensor3d_{}.cfg".format(queueio_name)

#     # Only generate file content on one rank:
#     if comm.Get_rank() == 0:
#         # First, create a file to write to:
#         file_name = str(tmpdir + "/test_queueio_sparsetensor3d_{}.h5".format(queueio_name))

#         # Next, write some sparsetensor3ds to that file:
#         create_sparsetensor3d_file(file_name, rand_num_events=25)

#         # Generate a config for this
#         config_contents = queue_io_sparsetensor3d_cfg_template.format(
#             name        = queueio_name,
#             input_files = file_name,
#             producer    = "test", #"sbndvoxels",
#             )

#         with open(str(config_file), 'w') as _f:
#             _f.write(config_contents)

#     # Put a barrier so that the config file and all contents are present:
#     comm.Barrier()

#     # Broadcast the config file and name so that all ranks know which one to use:
#     config_file = comm.bcast(config_file, root=0)
#     queueio_name = comm.bcast(queueio_name, root=0)

#     # Prepare data managers:
#     io_config = {
#         'filler_name' : queueio_name,
#         'filler_cfg'  : str(config_file),
#         'verbosity'   : 3,
#         'make_copy'   : make_copy
#     }

#     data_keys = OrderedDict({
#         'label': 'test_{}'.format(queueio_name),
#         })

#     li = distributed_queue_interface.queue_interface()

#     # Scale the number of images to the mpi comm size:
#     li.prepare_manager('primary', io_config, comm_size * local_batch_size, data_keys, color=0)

#     for i in range(n_reads):
#         data = li.fetch_minibatch_data('primary')
#         li.prepare_next('primary')
#         bs = local_batch_size
#         assert(data['label'].shape[0] == bs)


if __name__ == "__main__":
    test_sparsetensor3d_queueio("./", batch_size=1,  n_reads=10)
    test_sparsetensor3d_queueio("./", batch_size=2,  n_reads=10)
    # test_sparsetensor3d_queueio_distributed("./", local_batch_size=2, n_reads=10)
    print("Success")
