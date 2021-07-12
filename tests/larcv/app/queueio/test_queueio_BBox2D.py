import pytest
import unittest
import random
import uuid

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict

queue_io_bbox2d_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerBBox2D"]
  ProcessName:     ["test_{name}"]

  ProcessList: {{
    test_{name}: {{
      BBoxProducer: "{producer}"
      MaxBoxes: {max_boxes}
      UnfilledBoxValue: -999
      Channels: {channels}
    }}
  }}
}}
'''


def create_bbox2d_file(file_name, rand_num_events, n_projections):
    bbox_list = data_generator.create_bbox_list(rand_num_events, n_projections = n_projections, dimension = 2)
    data_generator.write_bboxes(file_name, bbox_list, dimension = 2, n_projections = n_projections)

    print(bbox_list)

@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [2])
@pytest.mark.parametrize('n_projections', [1,2])
@pytest.mark.parametrize('from_dense', [False, True])
def test_bbox2d_queueio(tmpdir, make_copy, batch_size, n_projections, from_dense, n_reads=10):


    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_bbox2d_{}.h5".format(queueio_name))

    # Next, write some bbox2ds to that file:
    create_bbox2d_file(file_name, rand_num_events=25, n_projections=n_projections)

    # Generate a config for this
    channels = list(range(n_projections))
    config_contents = queue_io_bbox2d_cfg_template.format(
        name        = queueio_name,
        input_files = file_name,
        producer    = "test",
        channels    = channels,
        max_boxes   = 25
        )

    config_file = tmpdir + "/test_queueio_bbox2d_{}.cfg".format(queueio_name)

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
        'boxes': 'test_{}'.format(queueio_name),
        })

    print("here")

    li = queueloader.queue_interface()
    li.prepare_manager('primary', io_config, batch_size, data_keys)

    for i in range(n_reads):
        data = li.fetch_minibatch_data('primary', pop=True)
        li.prepare_next('primary')
        assert(data['boxes'].shape[0] == batch_size)

if __name__ == "__main__":
    test_bbox2d_queueio("./", make_copy=False, batch_size=2, n_projections=1, from_dense=False, n_reads=10)
    test_bbox2d_queueio("./", make_copy=False, batch_size=2, n_projections=2, from_dense=False, n_reads=10)
    print("Success")