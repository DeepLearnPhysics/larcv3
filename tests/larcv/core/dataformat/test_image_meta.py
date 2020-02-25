import unittest
import numpy
import pytest

import larcv

from random import Random
random = Random()

N_CHECKS = 1

def image_meta_factory(dimension):
    if dimension == 1:
        return larcv.ImageMeta1D()
    elif dimension == 2:
        return larcv.ImageMeta2D()
    elif dimension == 3:
        return larcv.ImageMeta3D()
    elif dimension == 4:
        return larcv.ImageMeta4D()
    else:
        raise Expection("Can't do anything with dimension == ", dimension)


@pytest.mark.parametrize('dimension', [1,2,3,4])
def test_import(dimension):
    
    try:
        im = image_meta_factory(dimension)
    except:
        assert False

@pytest.mark.parametrize('dimension', [1,2,3,4])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_default_constructor(dimension, execution_number):
    
    im = image_meta_factory(dimension)

    # n_dims = random.randint(1,4)

    total_volume = 1.0
    total_voxels = 1

    _id = random.randint(1, 100)
    im.set_projection_id(_id)

    for dim in range(dimension):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        O = random.uniform(0.001, 1e4)
        im.set_dimension(dim, L, N, O)
        total_volume *= L
        total_voxels *= N

    assert(im.id() == _id)
    assert(im.n_dims() == dimension)
    assert(im.total_voxels() == total_voxels)



@pytest.mark.parametrize('dimension', [1,2,3,4])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_filled_constructor(dimension, execution_number):

    

    projection_id = random.randint(0,10)

    number_of_voxels = []
    image_sizes = []
    origin = []

    total_volume = 1.0
    total_voxels = 1

    for dim in range(dimension):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        O = random.uniform(0.001, 1e4)
        number_of_voxels.append(N)
        image_sizes.append(L)
        origin.append(O)
        total_volume *= L
        total_voxels *= N

    if dimension == 1:
        im = larcv.ImageMeta1D(projection_id, number_of_voxels, image_sizes)
    elif dimension == 2:
        im = larcv.ImageMeta2D(projection_id, number_of_voxels, image_sizes)
    elif dimension == 3:
        im = larcv.ImageMeta3D(projection_id, number_of_voxels, image_sizes)
    elif dimension == 4:
        im = larcv.ImageMeta4D(projection_id, number_of_voxels, image_sizes)
    else:
        assert(False)

    assert(im.n_dims() == dimension)
    assert(im.total_voxels() == total_voxels)

@pytest.mark.parametrize('dimension', [1,2,3,4])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_unravel(dimension, execution_number):

    im = image_meta_factory(dimension)    


    total_volume = 1.0
    total_voxels = 1

    dims = []

    for dim in range(dimension):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        im.set_dimension(dim, L, N)
        total_volume *= L
        total_voxels *= N
        dims.append(N)

    # Verify that the same uniform index gets unraveled to the same value as numpy:
    for i in range(50):
        flat_index = random.randint(0, total_voxels-1)
        np_unraveled = numpy.unravel_index(flat_index, dims)
        im_unraveled = im.coordinates(flat_index)

        for d in range(dimension):
            assert(np_unraveled[d] == im_unraveled[d])

@pytest.mark.parametrize('dimension', [1,2,3,4])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_ravel(dimension, execution_number):

    
    im = image_meta_factory(dimension)    

    total_volume = 1.0
    total_voxels = 1

    dims = []

    for dim in range(dimension):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        im.set_dimension(dim, L, N)
        total_volume *= L
        total_voxels *= N
        dims.append(N)

    # Verify that the same coordinates get raveled to the same index as numpy:
    for i in range(50):
        
        # Create a list of indexes:
        indexes = []
        vec_of_indexes = []
        for dim in dims:
            ind = random.randint(0, dim-1)
            indexes.append(ind)
            vec_of_indexes.append(ind)

        # for 
        np_raveled = numpy.ravel_multi_index(indexes, dims)
        im_raveled = im.index(vec_of_indexes)

        for d in range(dimension):
            assert(np_raveled == im_raveled)


@pytest.mark.parametrize('dimension', [1,2,3,4])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_compress(dimension, execution_number):

    
    im = image_meta_factory(dimension)    


    dims = []
    lens = []
    comp = []


    for dim in range(dimension):
        N = random.randint(1, 2e4)
        L = random.uniform(0.001, 1e4)
        im.set_dimension(dim, L, N)
        dims.append(N)
        lens.append(L)


    compression = [2]*dimension
    compressed_meta = im.compress(compression)

    # Verify that the same coordinates get raveled to the same index as numpy:
    for i in range(dimension):
        assert compressed_meta.image_size(i) == im.image_size(i) / compression[i]
        assert compressed_meta.number_of_voxels(i) == int(im.number_of_voxels(i) / compression[i])
        assert compressed_meta.origin(i) == im.origin(i) / compression[i]
