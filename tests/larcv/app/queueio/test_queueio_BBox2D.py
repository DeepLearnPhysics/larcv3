import pytest
import unittest
import random
import uuid
import numpy, time

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict

# queue_io_bbox2d_cfg_template = '''
# {name}: {{
#   Verbosity:       2
#   EnableFilter:    false
#   RandomAccess:    0
#   RandomSeed:      0
#   InputFiles:      [{input_files}]
#   ProcessType:     ["BatchFillerBBox2D"]
#   ProcessName:     ["test_{name}"]
#
#   ProcessList: {{
#     test_{name}: {{
#       BBoxProducer: "{producer}"
#       MaxBoxes: {max_boxes}
#       UnfilledBoxValue: -999
#       Channels: {channels}
#     }}
#   }}
# }}
# '''


def create_bbox2d_file(file_name, rand_num_events, n_projections):
    bbox_list, meta_list = data_generator.create_bbox_list(rand_num_events, n_projections = n_projections, dimension = 2)
    data_generator.write_bboxes(file_name, bbox_list, meta_list, dimension = 2, n_projections = n_projections)


@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [2])
@pytest.mark.parametrize('n_projections', [1,2])
@pytest.mark.parametrize('from_dense', [False, True])
def test_bbox2d_queueio(tmpdir, make_copy, batch_size, n_projections, from_dense, n_reads=10):





    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_bbox_{}.h5".format(queueio_name))

    # Next, write some bbox2ds to that file:
    create_bbox2d_file(file_name, rand_num_events=25, n_projections=n_projections)

    # Now, let's get the configuration of a queueio object:

    default_config = larcv.QueueProcessor.default_config()
    default_config["InputFiles"].append(file_name)

    # Add the batch filler to the default config:
    default_config["Verbosity"] = 0
    default_config["ProcessDriver"]["ProcessName"].append(f"test_{queueio_name}")
    default_config["ProcessDriver"]["ProcessType"].append("BatchFillerBBox2D")


    if default_config["ProcessDriver"]["ProcessList"] is None:
      default_config["ProcessDriver"]["ProcessList"] = {
        f"test_{queueio_name}":
          {
            "Producer": "test",
            "Channels": list(range(n_projections)),
          }
        }
    else:
      default_config["ProcessDriver"]["ProcessList"].append(
        {f"test_{queueio_name}":
          {
            "Producer": "test",
            "Channels": list(range(n_projections)),
          }
        })



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
    test_bbox2d_queueio("./", make_copy=False, batch_size=2, n_projections=1, from_dense=False, n_reads=10)
    test_bbox2d_queueio("./", make_copy=False, batch_size=2, n_projections=2, from_dense=False, n_reads=10)
    print("Success")
