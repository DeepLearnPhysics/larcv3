import pytest
import unittest
import random

@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test_sparse.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)


def write_sparse_tensors(tempfile, rand_num_events):

    from larcv import dataformat

    io_manager = dataformat.IOManager(dataformat.IOManager.kWRITE)
    io_manager.set_out_file(tempfile)
    io_manager.initialize()


    dimension = 2
    n_projections = 3

    meta_list = []
    for projection in range(n_projections):
        meta_list.append(dataformat.ImageMeta())
        for dim in range(dimension):
            L = random.uniform(0.001, 1e4)
            N = random.randint(1, 2e4)
            meta_list[-1].add_dimension(L, N)

        meta_list[-1].set_projection_id(projection)

        
    for i in range(rand_num_events):
        io_manager.set_id(1001, 0, i)
        
        # Get a piece of data, sparse tensor:
        ev_sparse = dataformat.EventSparseTensor.to_sparse_tensor(io_manager.get_data("sparse","test"))
        for projection in range(n_projections):
            vs = dataformat.VoxelSet()
            n_voxels = 10
            for j in range(n_voxels):
                vs.emplace(j, j, False)

            ev_sparse.set(vs, meta_list[projection])

        io_manager.save_entry()

    print("Finished event loop")



    # assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_sparse_tensors(tempfile):

    from larcv import dataformat

    io_manager = dataformat.IOManager(dataformat.IOManager.kREAD)
    io_manager.add_in_file(tempfile)
    io_manager.initialize()

    read_events = 0
    for i in range(io_manager.get_n_entries()):
        io_manager.read_entry(i)
        # print(io_manager.current_entry())
        event_id = io_manager.event_id()

        ev_particles = dataformat.EventParticle.to_particle(io_manager.get_data('particle', 'test'))
        read_events += 1

    return read_events

def test_write_sparse_tensors(tempfile, rand_num_events):
    pass
    # write_particles(tempfile, rand_num_events)
    # n_read = read_particles(tempfile)

    # assert(n_read == rand_num_events)

