import pytest
import unittest
import random

from larcv import larcv, data_generator

@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(1, 10)


def create_particle_file(file_name, rand_num_events):


    data_generator.write_particles(file_name, rand_num_events, particles_per_event=1)


def test_particle_threadio(tempfile):




def test_write_particles(tmpdir, rand_num_events):

    tempfile = str(tmpdir + "/test_write_particles.h5")

    data_generator.write_particles(tempfile, rand_num_events)
    n_read = data_generator.read_particles(tempfile)

    assert(n_read == rand_num_events)