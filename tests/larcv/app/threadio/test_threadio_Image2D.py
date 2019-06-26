import pytest
import unittest
import random
import uuid
import time

from larcv import larcv, data_generator, larcv_interface, distributed_larcv_interface

from collections import OrderedDict

thread_io_image2d_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0 
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerImage2D"]
  ProcessName:     ["test_{name}"]
  NumThreads:      {num_threads}
  NumBatchStorage: {num_storage}

  ProcessList: {{
    test_{name}: {{
      ImageProducer: "{producer}"
      Channels: {channels}
    }}
  }}
}}
'''


def create_image2d_file(file_name, rand_num_events, n_projections):
    event_image_list = data_generator.build_image2d(rand_num_events, n_projections = n_projections, shape=[64,64])
    data_generator.write_image2d(file_name, event_image_list)


@pytest.mark.parametrize('num_storage', [1, 2])
@pytest.mark.parametrize('num_threads', [1, 2])
@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [1, 2])
@pytest.mark.parametrize('n_projections', [1, 2])
def test_image2d_threadio(tmpdir, num_threads, num_storage, make_copy, batch_size, n_projections, n_reads=10):


    threadio_name = "threadio_{}".format(uuid.uuid4())

    # First, create a file to write to:
    file_name = str(tmpdir + "/test_threadio_image2ds_{}.h5".format(threadio_name))

    random_file_name = str(tmpdir + "/test_write_read_image2d.h5")

    # Next, write some image2ds to that file:
    create_image2d_file(file_name, rand_num_events=25, n_projections=n_projections)

    channels = list(range(n_projections))

    # Generate a config for this 

    config_contents = thread_io_image2d_cfg_template.format(
        name        = threadio_name,
        input_files = file_name,
        num_threads = num_threads,
        num_storage = num_storage,
        producer    = "test",
        channels    = channels,
        )

    config_file = tmpdir + "/test_threadio_image2ds_{}.cfg".format(threadio_name) 

    with open(str(config_file), 'w') as _f:
        _f.write(config_contents)

    # Prepare data managers:
    io_config = {
        'filler_name' : threadio_name,
        'filler_cfg'  : str(config_file),
        'verbosity'   : 3,
        'make_copy'   : make_copy
    }

    data_keys = OrderedDict({
        'label': 'test_{}'.format(threadio_name), 
        })



    li = larcv_interface.larcv_interface()
    li.prepare_manager('primary', io_config, batch_size, data_keys)


    for i in range(n_reads):
        data = li.fetch_minibatch_data('primary')
        assert(data['label'].shape[0] == batch_size)

    li.stop()


# @pytest.mark.distributed_test
# @pytest.mark.parametrize('num_storage', [1, 2])
# @pytest.mark.parametrize('num_threads', [1, 2])
# @pytest.mark.parametrize('make_copy', [True, False])
# @pytest.mark.parametrize('batch_size', [2, 4])
# @pytest.mark.parametrize('n_projections', [1, 2])
# def test_image2d_threadio_distributed(tmpdir, num_threads, num_storage, make_copy, batch_size, n_projections, n_reads=10):

#     from mpi4py import MPI
#     comm = MPI.COMM_WORLD

#     root_rank = comm.Get_size() - 1

#     threadio_name = "threadio_{}".format(uuid.uuid4())

#     # First, create a file to write to:
#     file_name = str(tmpdir + "/test_threadio_image2ds_{}.h5".format(threadio_name))

#     random_file_name = str(tmpdir + "/test_write_read_image2d.h5")

#     # Next, write some image2ds to that file:
#     if (comm.Get_rank() == root_rank):
#         create_image2d_file(file_name, rand_num_events=25, n_projections=n_projections)

#     channels = list(range(n_projections))

#     # Generate a config for this 
#     config_contents = thread_io_image2d_cfg_template.format(
#         name        = threadio_name,
#         input_files = file_name,
#         num_threads = num_threads,
#         num_storage = num_storage,
#         producer    = "test",
#         channels    = channels,
#         )

#     config_file = tmpdir + "/test_threadio_image2ds_{}.cfg".format(threadio_name) 

#     with open(str(config_file), 'w') as _f:
#         _f.write(config_contents)

#     # Prepare data managers:
#     io_config = {
#         'filler_name' : threadio_name,
#         'filler_cfg'  : str(config_file),
#         'verbosity'   : 3,
#         'make_copy'   : make_copy
#     }

#     data_keys = OrderedDict({
#         'label': 'test_{}'.format(threadio_name), 
#         })



#     li = distributed_larcv_interface.larcv_interface(root=root_rank)
#     li.prepare_manager('primary', io_config, batch_size, data_keys)


#     for i in range(n_reads):
#         data = li.fetch_minibatch_data('primary')
#         assert(data['label'].shape[0] == batch_size/comm.Get_size())

#     li.stop()

if __name__ == "__main__":
    test_image2d_threadio("./", num_threads=1, num_storage=1, make_copy=False, batch_size=2, n_projections=1, n_reads=10)
    test_image2d_threadio("./", num_threads=1, num_storage=2, make_copy=False, batch_size=2, n_projections=3, n_reads=10)
    print("Success")
