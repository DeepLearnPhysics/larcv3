import pytest
import unittest
import random
import larcv

from larcv import data_generator

@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test_particle.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)




def test_write_particles(tmpdir, rand_num_events):

    tempfile = str(tmpdir + "/test_write_particles.h5")

    data_generator.write_particles(tempfile, rand_num_events)
    n_read = data_generator.read_particles(tempfile)

    assert(n_read == rand_num_events)

# def test_read_particles_coredriver(tmpdir, rand_num_events):

#     tempfile = str(tmpdir + "/test_write_particles.h5")

#     write_particles(tempfile, rand_num_events)
#     n_read = read_particles(tempfile, True)
#     assert(n_read == rand_num_events)

if __name__ == '__main__':
    tmpdir = "./"
    rand_num_events = 5
    test_write_particles(tmpdir, rand_num_events)
