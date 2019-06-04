import pytest
import unittest
import random
from larcv import larcv

@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test_particle.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)


def write_particles(tempfile, rand_num_events):

    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    io_manager.set_out_file(tempfile)
    io_manager.initialize()



    for i in range(rand_num_events):
        print(io_manager.current_entry())
        io_manager.set_id(1001, 0, i)
        
        # Get a piece of data, particle:
        ev_particle = larcv.EventParticle.to_particle(io_manager.get_data("particle","test"))

        print(type(ev_particle))

        for j in range(i+1):
            part = larcv.Particle()
            part.energy_deposit(j)
            ev_particle.emplace_back(part)

        print(ev_particle.size())

        io_manager.save_entry()



    assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_particles(tempfile, use_core_driver=False):

    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    if use_core_driver: 
      io_manager.set_core_driver()
    io_manager.add_in_file(tempfile)
    io_manager.initialize()

    read_events = 0
    for i in range(io_manager.get_n_entries()):
        io_manager.read_entry(i)
        # print(io_manager.current_entry())
        event_id = io_manager.event_id()

        ev_particles = larcv.EventParticle.to_particle(io_manager.get_data('particle', 'test'))
        read_events += 1

    return read_events


def test_write_particles(tmpdir, rand_num_events):

    tempfile = str(tmpdir + "/test_write_particles.h5")

    write_particles(tempfile, rand_num_events)
    n_read = read_particles(tempfile)

    assert(n_read == rand_num_events)

# def test_read_particles_coredriver(tmpdir, rand_num_events):

#     tempfile = str(tmpdir + "/test_write_particles.h5")

#     write_particles(tempfile, rand_num_events)
#     n_read = read_particles(tempfile, True)
#     assert(n_read == rand_num_events)

if __name__ == '__main__':
    tmpdir = "./particle_test.h5"
    rand_num_events = 5
    test_write_particles(tmpdir, rand_num_events)
