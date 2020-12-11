import pytest
import unittest
import random
import uuid
import numpy
import time

import larcv
from larcv import  data_generator

from collections import OrderedDict


def create_particle_file(file_name, rand_num_events):
    data_generator.write_particles(file_name, rand_num_events, particles_per_event=1)


@pytest.mark.parametrize('batch_size', [1, 2])
def test_particle_queueio(tmpdir, batch_size, n_reads=10):

    queueio_name = "queueio_{}".format(uuid.uuid4())


    # First, create a file to write to:
    file_name = str(tmpdir + "/test_queueio_particles_{}.h5".format(queueio_name))

    # Next, write some particles to that file:
    create_particle_file(file_name, rand_num_events=25)

    # Now, let's get the configuration of a queueio object:

    default_config = larcv.QueueProcessor.default_config()
    default_config["InputFiles"].append(file_name)

    # Add the batch filler to the default config:
    default_config["Verbosity"] = 0
    default_config["ProcessDriver"]["ProcessName"].append(f"test_{queueio_name}")
    default_config["ProcessDriver"]["ProcessType"].append("BatchFillerParticle")

    
    if default_config["ProcessDriver"]["ProcessList"] is None:
      default_config["ProcessDriver"]["ProcessList"] = {
        f"test_{queueio_name}": 
          {
            "ParticleProducer": "test",
          }
        }
    else:
      default_config["ProcessDriver"]["ProcessList"].append(
        {f"test_{queueio_name}": 
          {
            "ParticleProducer": "test",
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
      # get the data?
      


if __name__ == "__main__":
    test_particle_queueio("./", batch_size=2, n_reads=10)
    test_particle_queueio("./", batch_size=2, n_reads=10)
    print("Success")
