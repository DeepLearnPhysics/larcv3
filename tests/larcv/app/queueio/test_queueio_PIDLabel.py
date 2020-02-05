import pytest
import unittest
import random
import uuid

import larcv
from larcv import queueloader,  data_generator

from collections import OrderedDict

queue_io_particle_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0 
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerPIDLabel"]
  ProcessName:     ["test_{name}"]

  ProcessList: {{
    test_{name}: {{
      ParticleProducer: "{producer}"
      PdgClassList: [0]
    }}
  }}
}}
'''


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


    # Generate a config for this 

    config_contents = queue_io_particle_cfg_template.format(
        name        = queueio_name,
        input_files = file_name,
        producer    = "test",
        )

    config_file = tmpdir + "/test_queueio_particles_{}.cfg".format(queueio_name) 

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
        assert(data['label'].shape[0] == batch_size)


if __name__ == "__main__":
    test_particle_queueio("./", make_copy=False, batch_size=2, n_reads=10)
    test_particle_queueio("./", make_copy=False, batch_size=2, n_reads=10)
    print("Success")
