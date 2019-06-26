import pytest
import unittest
import random
import uuid

from larcv import larcv, data_generator, larcv_interface

from collections import OrderedDict

thread_io_sparsetensor3d_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0 
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerSparseTensor3D"]
  ProcessName:     ["test_{name}"]
  NumThreads:      {num_threads}
  NumBatchStorage: {num_storage}

  ProcessList: {{
    test_{name}: {{
      Tensor3DProducer: "{producer}"
      MaxVoxels: 100
      UnfilledVoxelValue: -999
    }}
  }}
}}
'''


def create_sparsetensor3d_file(file_name, rand_num_events, n_projections=1):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=3, n_projections=n_projections)




@pytest.mark.parametrize('num_storage', [1,2])
@pytest.mark.parametrize('num_threads', [1,2])
@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [2])
def test_sparsetensor3d_threadio(tmpdir, num_threads, num_storage, make_copy, batch_size, n_reads=10):


    threadio_name = "threadio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_threadio_sparsetensor3d_{}.h5".format(threadio_name))

    # Next, write some sparsetensor3ds to that file:
    create_sparsetensor3d_file(file_name, rand_num_events=25)


    # Generate a config for this 
    config_contents = thread_io_sparsetensor3d_cfg_template.format(
        name        = threadio_name,
        input_files = file_name,
        num_threads = num_threads,
        num_storage = num_storage,
        producer    = "test",
        )

    config_file = tmpdir + "/test_threadio_sparsetensor3d_{}.cfg".format(threadio_name) 

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


@pytest.mark.distributed_test
@pytest.mark.parametrize('num_storage', [1,2])
@pytest.mark.parametrize('num_threads', [1,2])
@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('local_batch_size', [2])
@pytest.mark.parametrize('read_option', ['read_from_single_rank', 'read_from_all_ranks'])
def test_sparsetensor3d_threadio_distributed(tmpdir, num_threads, num_storage, make_copy, local_batch_size, read_option, n_reads=10):

    from larcv import distributed_larcv_interface
    from mpi4py import MPI
    comm = MPI.COMM_WORLD

    root_rank = comm.Get_size() - 1
    comm_size = comm.Get_size()

    threadio_name = "threadio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_threadio_sparsetensor3d_{}.h5".format(threadio_name))

    # Next, write some sparsetensor3ds to that file:
    if (comm.Get_rank() == root_rank and read_option == 'read_from_single_rank'):
        create_sparsetensor3d_file(file_name, rand_num_events=25)
    if (read_option == 'read_from_all_ranks'):
        create_sparsetensor3d_file(file_name, rand_num_events=25)


    # Generate a config for this 
    config_contents = thread_io_sparsetensor3d_cfg_template.format(
        name        = threadio_name,
        input_files = file_name,
        num_threads = num_threads,
        num_storage = num_storage,
        producer    = "test",
        )

    config_file = tmpdir + "/test_threadio_sparsetensor3d_{}.cfg".format(threadio_name) 

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



    li = distributed_larcv_interface.larcv_interface(root=root_rank, read_option=read_option)
    # Scale the number of images to the mpi comm size:
    li.prepare_manager('primary', io_config, comm_size * local_batch_size, data_keys)


    for i in range(n_reads):
        data = li.fetch_minibatch_data('primary')
        bs = local_batch_size
        if read_option == 'read_from_all_ranks':
            bs *= comm.Get_size()
        assert(data['label'].shape[0] == bs)


if __name__ == "__main__":
    test_sparsetensor3d_threadio("./", num_threads=1, num_storage=1, make_copy=False, batch_size=2,  n_reads=10)
    test_sparsetensor3d_threadio("./", num_threads=1, num_storage=2, make_copy=False, batch_size=2,  n_reads=10)
    # test_sparsetensor3d_threadio_distributed("./", num_threads=1, num_storage=2, make_copy=False, batch_size=2, read_option='read_from_all_ranks', n_reads=10)
    print("Success")
