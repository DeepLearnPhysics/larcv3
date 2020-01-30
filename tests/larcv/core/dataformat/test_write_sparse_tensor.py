import pytest
import unittest
import larcv

from random import Random
random = Random()

from larcv import data_generator

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)



@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_write_sparse_tensors(tmpdir, rand_num_events, dimension, n_projections):

    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)

    random_file_name = str(tmpdir + "/test_write_sparse_tensors.h5")

    data_generator.write_sparse_tensors(random_file_name, voxel_set_list, dimension, n_projections)


@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_read_write_sparse_tensors(tmpdir, rand_num_events, dimension, n_projections):

    import numpy

    random_file_name = str(tmpdir + "/test_write_read_sparse_tensors.h5")

    voxel_set_list = data_generator.build_sparse_tensor(rand_num_events, n_projections = n_projections)

    data_generator.write_sparse_tensors(random_file_name, voxel_set_list, dimension, n_projections)
    read_voxel_set_list = data_generator.read_sparse_tensors(random_file_name, dimension)

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








