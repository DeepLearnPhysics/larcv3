import pytest
import unittest
import random
import uuid

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict

queue_io_tensor2d_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerTensor2D"]
  ProcessName:     ["test_{name}"]

  ProcessList: {{
    test_{name}: {{
      TensorProducer: "{producer}"
      TensorType: "{type}"
      Channels: {channels}
    }}
  }}
}}
'''


def create_tensor2d_file(file_name, rand_num_events, n_projections):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=2, n_projections=n_projections)

def create_dense_tensor2d_file(file_name, rand_num_events, n_projections):
    voxel_set_list = data_generator.build_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_tensor(file_name, voxel_set_list, dimension=2)

@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [2])
@pytest.mark.parametrize('n_projections', [1,2])
@pytest.mark.parametrize('from_dense', [False, True])
def test_tensor2d_queueio(tmpdir, make_copy, batch_size, n_projections, from_dense, n_reads=10):


    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_tensor2d_{}.h5".format(queueio_name))

    # Next, write some tensor2ds to that file:
    if from_dense:
        create_dense_tensor2d_file(file_name, rand_num_events=25, n_projections=n_projections)
    else:
        create_tensor2d_file(file_name, rand_num_events=25, n_projections=n_projections)


    if from_dense:
        tensor_type = "dense"
    else:
        tensor_type = "sparse"

    # Generate a config for this
    channels = list(range(n_projections))
    config_contents = queue_io_tensor2d_cfg_template.format(
        name        = queueio_name,
        input_files = file_name,
        producer    = "test",
        type        = tensor_type,
        channels    = channels,
        )

    config_file = tmpdir + "/test_queueio_tensor2d_{}.cfg".format(queueio_name)

    with open(str(config_file), 'w') as _f:
        _f.write(config_contents)

    # Prepare data managers:
    io_config = {
        'filler_name' : queueio_name,
        'filler_cfg'  : str(config_file),
        'verbosity'   : 3,
        'make_copy'   : make_copy
    }

    data_keys = OrderedDict({
        'label': 'test_{}'.format(queueio_name),
        })

    print("here")

    li = queueloader.queue_interface()
    li.prepare_manager('primary', io_config, batch_size, data_keys)

    for i in range(n_reads):
        data = li.fetch_minibatch_data('primary', pop=True)
        li.prepare_next('primary')
        assert(data['label'].shape[0] == batch_size)


if __name__ == "__main__":
    test_tensor2d_queueio("./", make_copy=False, batch_size=2, n_projections=1, from_dense=False, n_reads=10)
    test_tensor2d_queueio("./", make_copy=False, batch_size=2, n_projections=2, from_dense=False, n_reads=10)
    print("Success")