import pytest
import unittest
import random
import uuid
import numpy

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict


def create_particle_file(file_name, rand_num_events):
    data_generator.write_particles(file_name, rand_num_events, particles_per_event=1)


@pytest.mark.parametrize('make_copy', [True, False])
@pytest.mark.parametrize('batch_size', [2])
def test_particle_queueio(tmpdir, make_copy, batch_size, n_reads=10):

    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_particles_{}.h5".format(queueio_name))

    # Next, write some particles to that file:
    create_particle_file(file_name, rand_num_events=25)

    # Now, let's get the configuration of a queueio object:

    default_config = larcv.QueueProcessor.default_config()
    print(default_config)
    default_config["InputFiles"].append(file_name)

    # Add the batch filler to the default config:
    print(default_config["ProcessName"])
    print(type(default_config["ProcessName"]))
    default_config["Verbosity"] = 0
    default_config["ProcessDriver"]["ProcessName"].append(f"test_{queueio_name}")
    default_config["ProcessDriver"]["ProcessType"].append("BatchFillerPIDLabel")

    print(default_config)
    
    if default_config["ProcessDriver"]["ProcessList"] is None:
      default_config["ProcessDriver"]["ProcessList"] = {
        f"test_{queueio_name}": 
          {
            "ParticleProducer": "test",
            "PdgClassList": [0],
          }
        }
    else:
      default_config["ProcessDriver"]["ProcessList"].append(
        {f"test_{queueio_name}": 
          {
            "ParticleProducer": "test",
            "PdgClassList": [0],
          }
        })

    print(default_config)


    queue_proc = larcv.QueueProcessor()

    queue_proc.configure(default_config)

    # Generate a config for this 

    # config_contents = queue_io_particle_cfg_template.format(
    #     name        = queueio_name,
    #     input_files = file_name,
    #     producer    = "test",
    #     )

    # config_file = tmpdir + "/test_queueio_particles_{}.cfg".format(queueio_name) 

    # with open(str(config_file), 'w') as _f:
    #     _f.write(config_contents)

    # # Prepare data managers:
    # io_config = {
    #     'filler_name' : queueio_name,
    #     'filler_cfg'  : str(config_file),
    #     'verbosity'   : 3,
    #     'make_copy'   : make_copy
    # }

    # data_keys = OrderedDict({
    #     'label': 'test_{}'.format(queueio_name), 
    #     })

    indexes = numpy.arange(batch_size*n_reads*2) % queue_proc.get_n_entries()

    print(indexes)
    queue_proc.set_next_batch(indexes[0:batch_size])
    # print("set")
    # queue_proc.prepare_next()
    # print("prepared")
    # sleep(10)
    for i in range(n_reads):
      queue_proc.batch_process()
      while queue_proc.is_reading():
        print("Sleeping")
        sleep(0.1)
      batch=indexes[i*batch_size:(i+1)*batch_size]
      queue_proc.set_next_batch(batch)

      # queue_proc.pop_current_data()
      # print("Popped")
      # queue_proc.set_next_batch(batch)
      # print("Set")
      # queue_proc.prepare_next()
      # print("Prepared")



if __name__ == "__main__":
    test_particle_queueio("./", make_copy=False, batch_size=2, n_reads=10)
    test_particle_queueio("./", make_copy=False, batch_size=2, n_reads=10)
    print("Success")
