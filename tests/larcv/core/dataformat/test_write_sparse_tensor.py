import pytest
import unittest
from larcv import larcv

from random import Random
random = Random()



@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)


def write_sparse_tensors(tempfile, voxel_set_list, dimension, n_projections):


    from copy import copy
    io_manager = larcv.IOManager(larcv.IOManager.kWRITE)
    print(type(tempfile))
    io_manager.set_out_file(tempfile)
    io_manager.initialize()

    # For this test, the meta is pretty irrelevant as long as it is consistent
    meta_list = []
    for projection in range(n_projections):
        if dimension == 2:
            meta_list.append(larcv.ImageMeta2D())
        else:
            meta_list.append(larcv.ImageMeta3D())

        for dim in range(dimension):
            L = 10.
            N = 512
            meta_list[-1].set_dimension(dim, L, N)

        meta_list[-1].set_projection_id(projection)

    for i in range(len(voxel_set_list)):
        io_manager.set_id(1001, 0, i)
        
        # Get a piece of data, sparse tensor:
        if dimension== 2:
            ev_sparse = larcv.EventSparseTensor2D.to_sparse_tensor(io_manager.get_data("sparse2d","test"))
        else:
            ev_sparse = larcv.EventSparseTensor3D.to_sparse_tensor(io_manager.get_data("sparse3d","test"))


        for projection in range(n_projections):
            vs = larcv.VoxelSet()
            indexes = voxel_set_list[i][projection]['indexes']
            values = voxel_set_list[i][projection]['values']
            for j in range(voxel_set_list[i][projection]['n_voxels']):
                vs.emplace(indexes[j], values[j], False)

            ev_sparse.set(vs, copy(meta_list[projection]))
        io_manager.save_entry()


    print("Finished event loop")



    # assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_sparse_tensors(tempfile, dimension):



    io_manager = larcv.IOManager(larcv.IOManager.kREAD)
    io_manager.add_in_file(tempfile)
    io_manager.initialize()


    voxel_set_list = []
     
    for i in range(io_manager.get_n_entries()):
        voxel_set_list.append([])

        io_manager.read_entry(i)
        
        # Get a piece of data, sparse tensor:
        if dimension== 2:
            ev_sparse = larcv.EventSparseTensor2D.to_sparse_tensor(io_manager.get_data("sparse2d","test"))
        else:
            ev_sparse = larcv.EventSparseTensor3D.to_sparse_tensor(io_manager.get_data("sparse3d","test"))

        for projection in range(ev_sparse.size()):
            voxel_set_list[i].append({
                "indexes" : [],
                "values"  : [],
                "n_voxels": ev_sparse.sparse_tensor(projection).size()
                })
            for j in range(ev_sparse.sparse_tensor(projection).size()):
                voxel_set_list[i][projection]['indexes'].append(ev_sparse.sparse_tensor(projection).as_vector()[j].id())
                voxel_set_list[i][projection]['values'].append(ev_sparse.sparse_tensor(projection).as_vector()[j].value())



    return voxel_set_list

def build_sparse_tensor(rand_num_events, n_projections=3):

    voxel_set_list = []
     
    for i in range(rand_num_events):
        voxel_set_list.append([])
        # Get a piece of data, sparse tensor:
        for projection in range(n_projections):
            n_voxels = random.randint(0,50)
            voxel_set_list[i].append({
                'values'  : [],
                'indexes' : random.sample(range(512*512), n_voxels),
                'n_voxels': n_voxels
                })
            for j in range(voxel_set_list[i][projection]['n_voxels']):
                voxel_set_list[i][-1]['values'].append(random.uniform(-1e3, 1e3) )

    return voxel_set_list


@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_write_sparse_tensors(tmpdir, rand_num_events, dimension, n_projections):

    voxel_set_list = build_sparse_tensor(rand_num_events, n_projections = n_projections)

    random_file_name = str(tmpdir + "/test_write_sparse_tensors.h5")

    write_sparse_tensors(random_file_name, voxel_set_list, dimension, n_projections)


@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_read_write_sparse_tensors(tmpdir, rand_num_events, dimension, n_projections):

    import numpy

    random_file_name = str(tmpdir + "/test_write_read_sparse_tensors.h5")

    voxel_set_list = build_sparse_tensor(rand_num_events, n_projections = n_projections)

    write_sparse_tensors(random_file_name, voxel_set_list, dimension, n_projections)
    read_voxel_set_list = read_sparse_tensors(random_file_name, dimension)

    # Check the same number of events came back:
    assert(len(read_voxel_set_list) == rand_num_events)
    for event in range(rand_num_events):
        # Check the same number of projections per event:
        assert(len(read_voxel_set_list[event]) == len(voxel_set_list[event]))
        for projection in range(n_projections):
            # Check the same number of voxels:
            input_voxelset = voxel_set_list[event][projection]
            read_voxelset = read_voxel_set_list[event][projection]
            assert(read_voxelset['n_voxels'] == input_voxelset['n_voxels'])

            # Check voxel properties:
            # Sum of indexes
            # Sum of values
            # std of values
            if input_voxelset['n_voxels'] == 0:
                continue
            print(input_voxelset['values'])
            assert(numpy.sum(input_voxelset['indexes']) == numpy.sum(read_voxelset['indexes']))
            assert( abs( numpy.sum(input_voxelset['values']) - numpy.sum(read_voxelset['values']) ) < 1e-3 )
            assert( abs( numpy.std(input_voxelset['values']) - numpy.std(read_voxelset['values']) ) < 1e-3 )



if __name__ == '__main__':
    tmpdir = "./"
    rand_num_events = 5
    n_projections = 3
    dimension = 2
    test_read_write_sparse_tensors(tmpdir, rand_num_events, dimension, n_projections)








