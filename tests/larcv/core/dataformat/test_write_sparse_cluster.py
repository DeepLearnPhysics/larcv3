import pytest
import unittest


from random import Random
random = Random(1)


@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test_sparse.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)


def write_sparse_clusters(file_name, voxel_set_array_list, dimension=2, n_projections=3):

    from larcv import dataformat
    import copy

    io_manager = dataformat.IOManager(dataformat.IOManager.kWRITE)
    io_manager.set_out_file(file_name)
    io_manager.initialize()


    # For this test, the meta is pretty irrelevant as long as it is consistent
    meta_list = []
    for projection in range(n_projections):
        if dimension == 2:
            meta_list.append(dataformat.ImageMeta2D())
        else:
            meta_list.append(dataformat.ImageMeta3D())

        for dim in range(dimension):
            L = 10.
            N = 2048
            meta_list[-1].set_dimension(dim, L, N)

        meta_list[-1].set_projection_id(projection)

    for i in range(len(voxel_set_array_list)):
        io_manager.set_id(1001, 0, i)
        # Get a piece of data, sparse tensor:
        if dimension== 2:
            ev_cluster = dataformat.EventSparseCluster2D.to_sparse_cluster(io_manager.get_data("cluster2d","test"))
        else:
            ev_cluster = dataformat.EventSparseCluster3D.to_sparse_cluster(io_manager.get_data("cluster3d","test"))

        # Holder for the voxels to store:


        for projection in range(n_projections):
            clusters = voxel_set_array_list[i][projection]
            if dimension == 2:
                vsa = dataformat.SparseCluster2D()
            else:
                vsa = dataformat.SparseCluster3D()
            for cluster in range(len(clusters)):
                vs = dataformat.VoxelSet()

                vs.id(cluster)
                indexes = clusters[cluster]['indexes']
                values = clusters[cluster]['values']
                for j in range(clusters[cluster]['n_voxels']):
                    vs.emplace(indexes[j], values[j], False)
                vsa.emplace(vs)          
            # sparse_cluster.set(vsa, )
            vsa.meta(meta_list[projection])
            # ev_cluster.emplace(vsa, copy.copy(meta_list[projection]))
            ev_cluster.set(vsa)

        io_manager.save_entry()





    # assert(io_manager.get_n_entries_out() == rand_num_events)

    io_manager.finalize()

    return
 
def read_sparse_clusters(tempfile, dimension):

    from larcv import dataformat

    io_manager = dataformat.IOManager(dataformat.IOManager.kREAD)
    io_manager.add_in_file(tempfile)
    io_manager.initialize()


    voxel_set_array_list = []
     
    for event in range(io_manager.get_n_entries()):
        # append a list of projections for this event:
        voxel_set_array_list.append([])

        io_manager.read_entry(event)
        
        # Get a piece of data, sparse cluster:\
        if dimension == 2:
            ev_cluster = dataformat.EventSparseCluster2D.to_sparse_cluster(io_manager.get_data("cluster2d","test"))
        else:
            ev_cluster = dataformat.EventSparseCluster3D.to_sparse_cluster(io_manager.get_data("cluster3d","test"))

        for projection in range(ev_cluster.size()):
            # Append a list of clusters for this projection:
            voxel_set_array_list[event].append([])
            print("Number of clusters: ", ev_cluster.sparse_cluster(projection).size())
            sparse_cluster = ev_cluster.sparse_cluster(projection)
            print("Current voxel_set_array_list length: ", len(voxel_set_array_list))
            print("Current voxel_set_array_list[] length: ", len(voxel_set_array_list[event]))
            for cluster in range(sparse_cluster.size()):
                # Append a dict of values for this cluster
                voxel_set_array_list[event][projection].append({
                    "indexes" : [],
                    "values"  : [],
                    "n_voxels": sparse_cluster.voxel_set(cluster).size()
                    })
                for j in range(sparse_cluster.voxel_set(cluster).size()):
                    voxel_set_array_list[event][projection][cluster]['indexes'].append(sparse_cluster.voxel_set(cluster).as_vector()[j].id())
                    voxel_set_array_list[event][projection][cluster]['values'].append(sparse_cluster.voxel_set(cluster).as_vector()[j].value())



    return voxel_set_array_list

# @pytest.mark.parametrize()
@pytest.mark.parametrize('dimension', [2, 3])
@pytest.mark.parametrize('n_projections', [1, 2, 3])
def test_write_sparse_clusters(tmpdir, rand_num_events, dimension, n_projections):

    voxel_set_array_list = build_sparse_cluster_list(rand_num_events, n_projections)
    random_file_name = str(tmpdir + "/test_write_sparse_tensors.h5")
    write_sparse_clusters(random_file_name, voxel_set_array_list, dimension, n_projections)

def build_sparse_cluster_list(rand_num_events, n_projections=3):


    voxel_set_array_list = []
     
    for event in range(rand_num_events):
        #  Add space for an event:
        voxel_set_array_list.append([])
        # Get a piece of data, sparse tensor:
        for projection in range(n_projections):
            # In this event, add space for a projection:
            voxel_set_array_list[event].append([])
            n_clusters = random.randint(1,25)
            for cluster in range(n_clusters):
                # print(len(voxel_set_array_list[projection][cluster]))
                n_voxels = random.randint(1,50)
                cluster_d = {                    
                    'values'  : [],
                    'indexes' : random.sample(range(512*512), n_voxels),
                    'n_voxels': n_voxels}
                for j in range(n_voxels):
                    cluster_d['values'].append(random.uniform(-1e3, 1e3) )

                voxel_set_array_list[event][projection].append(cluster_d)

    return voxel_set_array_list

@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('n_projections', [1,2,3])
def test_read_write_sparse_clusters(tmpdir, rand_num_events, dimension, n_projections):
   
    import numpy
   

    voxel_set_array_list = build_sparse_cluster_list(rand_num_events, n_projections)
    random_file_name = str(tmpdir + "/test_write_sparse_tensors.h5")


    write_sparse_clusters(random_file_name, voxel_set_array_list, dimension, n_projections)
    read_voxel_set_array_list = read_sparse_clusters(random_file_name, dimension)

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










