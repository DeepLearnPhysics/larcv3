import pytest
import unittest
import random

import larcv

@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(1, 10)

def write_temp_file(tempfile, rand_num_events):


    # This function is purely to test the IO read write capabilities.
    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    io_manager.set_out_file(tempfile)
    io_manager.initialize()
    for i in range(rand_num_events):
        io_manager.set_id(0,0,i)
        io_manager.save_entry()
    io_manager.finalize()

def read_temp_file(tempfile, rand_num_events):

    # This function is purely to test the IO read write capabilities.
    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    io_manager.add_in_file(tempfile)
    io_manager.initialize()

    return io_manager.get_n_entries()


def test_file_open(tempfile, rand_num_events):

    write_temp_file(tempfile, rand_num_events)
    n_read = read_temp_file(tempfile, rand_num_events)

    assert(n_read == rand_num_events)
