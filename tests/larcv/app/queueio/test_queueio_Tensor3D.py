import pytest
import unittest
import random
import uuid

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict

queue_io_tensor3d_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0 
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerTensor3D"]
  ProcessName:     ["test_{name}"]

  ProcessList: {{
    test_{name}: {{
      Tensor3DProducer: "{producer}"
    }}
  }}
}}
'''


def create_tensor3d_file(file_name, rand_num_events, n_projections=1):
    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)
    data_generator.write_sparse_tensors(file_name, voxel_set_list, dimension=3, n_projections=n_projections)




@pytest.mark.parametrize('make_copy', [False])
@pytest.mark.parametrize('batch_size', [1,2])
def test_tensor3d_queueio(tmpdir, make_copy, batch_size, n_reads=2):


    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_tensor3d_{}.h5".format(queueio_name))

    # Next, write some tensor3ds to that file:
    create_tensor3d_file(file_name, rand_num_events=25)


    # Generate a config for this 
    config_contents = queue_io_tensor3d_cfg_template.format(
        name        = queueio_name,
        input_files = file_name,
        producer    = "test",
        )

    config_file = tmpdir + "/test_queueio_tensor3d_{}.cfg".format(queueio_name) 

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



    li = queueloader.queue_interface()
    li.prepare_manager('primary', io_config, batch_size, data_keys)


    for i in range(n_reads):
        data = li.fetch_minibatch_data('primary')
        li.prepare_next('primary')
        print(data['label'].shape)
        assert(data['label'].shape[0] == batch_size)


if __name__ == "__main__":
    test_tensor3d_queueio("./", make_copy=False, batch_size=1,  n_reads=2)
    print("Success")
