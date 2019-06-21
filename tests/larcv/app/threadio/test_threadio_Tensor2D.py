import pytest
import unittest
import random
import uuid

from larcv import larcv, data_generator, larcv_interface

from collections import OrderedDict

thread_io_tensor2d_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0 
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerTensor2D"]
  ProcessName:     ["test_{name}"]
  NumThreads:      {num_threads}
  NumBatchStorage: {num_storage}

  ProcessList: {{
    test_{name}: {{
      Tensor2DProducer: "{producer}"
      Channels: {channels}
    }}
  }}
}}
'''


def create_tensor2d_file(file_name, rand_num_events, n_projections):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=2, n_projections=n_projections)

@pytest.mark.parametrize('num_storage', [1,2])
@pytest.mark.parametrize('num_threads', [1,2])
@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [2])
@pytest.mark.parametrize('n_projections', [1,2])
def test_tensor2d_threadio(tmpdir, num_threads, num_storage, make_copy, batch_size, n_projections, n_reads=10):


    threadio_name = "threadio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_threadio_tensor2d_{}.h5".format(threadio_name))

    # Next, write some tensor2ds to that file:
    create_tensor2d_file(file_name, rand_num_events=25, n_projections=n_projections)


    # Generate a config for this 
    channels = list(range(n_projections))
    config_contents = thread_io_tensor2d_cfg_template.format(
        name        = threadio_name,
        input_files = file_name,
        num_threads = num_threads,
        num_storage = num_storage,
        producer    = "test",
        channels    = channels,
        )

    config_file = tmpdir + "/test_threadio_tensor2d_{}.cfg".format(threadio_name) 

    with open(config_file, 'w') as _f:
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


if __name__ == "__main__":
    test_tensor2d_threadio("./", num_threads=1, num_storage=1, make_copy=False, batch_size=2, n_projections=1, n_reads=10)
    test_tensor2d_threadio("./", num_threads=1, num_storage=2, make_copy=False, batch_size=2, n_projections=2, n_reads=10)
    print("Success")
