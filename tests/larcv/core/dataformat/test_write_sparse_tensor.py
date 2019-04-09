import pytest
import unittest


from random import Random
random = Random()


@pytest.fixture()
def tempfile(tmpdir):
    return str(tmpdir.dirpath() + "/test_sparse.h5")

@pytest.fixture()
def rand_num_events():
    return random.randint(5, 100)


def write_sparse_tensors(tempfile, voxel_set_list):

    from larcv import dataformat

    io_manager = dataformat.IOManager(dataformat.IOManager.kWRITE)
    io_manager.set_out_file(tempfile)
    io_manager.initialize()


    dimension = 2
    n_projections = 3

    # For this test, the meta is pretty irrelevant as long as it is consistent
    meta_list = []
    for projection in range(n_projections):
        meta_list.append(dataformat.ImageMeta())
        for dim in range(dimension):
            L = 10.
            N = 512
            meta_list[-1].add_dimension(L, N)

        meta_list[-1].set_projection_id(projection)

        
    for i in range(len(voxel_set_list)):
        io_manager.set_id(1001, 0, i)
        
        # Get a piece of data, sparse tensor:
        ev_sparse = dataformat.EventSparseTensor.to_sparse_tensor(io_manager.get_data("sparse","test"))

        for projection in range(n_projections):
            vs = dataformat.VoxelSet()
            indexes = voxel_set_list[i][projection]['indexes']
            values = voxel_set_list[i][projection]['values']
            for j in range(voxel_set_list[i][projection]['n_voxels']):
                vs.emplace(indexes[j], values[j], False)

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


    voxel_set_list = []
     
    for i in range(io_manager.get_n_entries()):
        voxel_set_list.append([])

        io_manager.read_entry(i)
        
        # Get a piece of data, sparse tensor:
        ev_sparse = dataformat.EventSparseTensor.to_sparse_tensor(io_manager.get_data("sparse","test"))

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

# @pytest.mark.parametrize('random_seed', random.rando(N_CHECKS))
def test_write_sparse_tensors(tempfile, rand_num_events):
    from random import Random
    random = Random()

    rand_num_events = 5
    n_projections = 3

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



    write_sparse_tensors(tempfile, voxel_set_list)

def test_read_write_sparse_tensors(tempfile, rand_num_events):
    from random import Random
    random = Random()
    import numpy

    n_projections = 3

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



    write_sparse_tensors("test_sparse.h5", voxel_set_list)
    read_voxel_set_list = read_sparse_tensors("test_sparse.h5")

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










