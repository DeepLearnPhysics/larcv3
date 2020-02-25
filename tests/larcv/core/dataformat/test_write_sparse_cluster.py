import pytest
import unittest
import larcv

from random import Random
random = Random(1)

from larcv import data_generator

@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test_sparse.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)



# @pytest.mark.parametrize()
@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_write_sparse_clusters(tmpdir, rand_num_events, dimension, n_projections):

    voxel_set_array_list = data_generator.build_sparse_cluster_list(rand_num_events, n_projections)
    random_file_name = str(tmpdir + "/test_write_sparse_clusters.h5")
    data_generator.write_sparse_clusters(random_file_name, voxel_set_array_list, dimension, n_projections)



@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('n_projections', [1,2,3])
def test_read_write_sparse_clusters(tmpdir, rand_num_events, dimension, n_projections):
   
    import numpy
   

    voxel_set_array_list = data_generator.build_sparse_cluster_list(rand_num_events, n_projections)
    random_file_name = str(tmpdir + "/test_write_sparse_clusters.h5")

    print(len(voxel_set_array_list))
    for e in voxel_set_array_list:
        print("  " + str(len(e)))
        for p in e:
            print("    " + str(len(p)))

    data_generator.write_sparse_clusters(random_file_name, voxel_set_array_list, dimension, n_projections)
    read_voxel_set_array_list = data_generator.read_sparse_clusters(random_file_name, dimension)

    # Check the same number of events came back:
    assert(len(read_voxel_set_array_list) == rand_num_events)
    for event in range(rand_num_events):
        # Check the same number of projections per event:
        print
        assert(len(read_voxel_set_array_list[event]) == len(voxel_set_array_list[event]))

        for projection in range(n_projections):
            # CHeck the number of clusters in this projection:
            assert(len(read_voxel_set_array_list[event][projection]) == len(voxel_set_array_list[event][projection]))

            for cluster in range(len(read_voxel_set_array_list[event][projection])):
                # Check the same number of voxels:
                input_voxelset = voxel_set_array_list[event][projection][cluster]
                read_voxelset = read_voxel_set_array_list[event][projection][cluster]
                assert(read_voxelset['n_voxels'] == input_voxelset['n_voxels'])

                # Check voxel properties:
                # Sum of indexes
                # Sum of values
                # std of values
                if input_voxelset['n_voxels'] == 0:
                    continue
                # print(input_voxelset['values'])
                assert(numpy.sum(input_voxelset['indexes']) == numpy.sum(read_voxelset['indexes']))
                assert( abs( numpy.sum(input_voxelset['values']) - numpy.sum(read_voxelset['values']) ) < 1e-3 )
                assert( abs( numpy.std(input_voxelset['values']) - numpy.std(read_voxelset['values']) ) < 1e-3 )




if __name__ == '__main__':
    tmpdir = "./"
    rand_num_events = 5
    n_projections = 3
    dimension = 2
    test_read_write_sparse_clusters(tmpdir, rand_num_events, dimension, n_projections)



