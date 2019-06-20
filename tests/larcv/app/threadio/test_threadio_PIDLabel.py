import pytest
import unittest
import random

from larcv import larcv, data_generator, larcv_interface

from collections import OrderedDict

thread_io_particle_cfg_template = '''
{name}: {{
  Verbosity:       2
  EnableFilter:    false
  RandomAccess:    0 
  RandomSeed:      0
  InputFiles:      [{input_files}]
  ProcessType:     ["BatchFillerPIDLabel"]
  ProcessName:     ["test_{name}"]
  NumThreads:      {num_threads}
  NumBatchStorage: {num_storage}

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


# @pytest.mark.parametrize('num_threads', [1,2])
@pytest.mark.parametrize('num_storage', [1,2])
# @pytest.mark.parametrize('make_copy', [True, False])
# @pytest.mark.parametrize('batch_size', [1,2,5])
@pytest.mark.parametrize('num_threads', [1])
# @pytest.mark.parametrize('num_storage', [1])
@pytest.mark.parametrize('make_copy', [True])
@pytest.mark.parametrize('batch_size', [1])
def test_particle_threadio(tmpdir, num_threads, num_storage, make_copy, batch_size, n_reads=10):


    threadio_name = "threadio_{}_{}_{}_{}_{}".format(num_threads,
        num_storage,
        make_copy,
        batch_size,
        n_reads)

    # First, create a file to write to:
    file_name = str(tmpdir + "/test_threadio_particles_{}.h5".format(threadio_name))

    # Next, write some particles to that file:
    create_particle_file(file_name, rand_num_events=25)


    # Generate a config for this 

    config_contents = thread_io_particle_cfg_template.format(
        name        = threadio_name,
        input_files = file_name,
        num_threads = num_threads,
        num_storage = num_storage,
        producer    = "test",
        )

    config_file = tmpdir + "/test_threadio_particles_{}.cfg".format(threadio_name) 

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
    test_particle_threadio("./", num_threads=1, num_storage=1, make_copy=False, batch_size=2, n_reads=10)
    test_particle_threadio("./", num_threads=1, num_storage=2, make_copy=False, batch_size=2, n_reads=10)
    print("Success")
