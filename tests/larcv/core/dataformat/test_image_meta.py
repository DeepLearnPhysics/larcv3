import unittest
import numpy
import pytest

from larcv import larcv

from random import Random
random = Random()

N_CHECKS = 1

@pytest.mark.parametrize('dimension', [2,3])
def test_import(dimension):
    
    if dimension == 2:
        im = larcv.ImageMeta2D()
    elif dimension == 3:
        im = larcv.ImageMeta3D()
    else:
        assert False

@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_default_constructor(dimension, execution_number):
    
    if dimension == 2:
        im = larcv.ImageMeta2D()
    elif dimension == 3:
        im = larcv.ImageMeta3D()
    else:
        assert(False)

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



@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_filled_constructor(dimension, execution_number):

    

    projection_id = random.randint(0,10)

    number_of_voxels = larcv.VectorOfSizet()
    image_sizes = larcv.VectorOfDouble()
    origin = larcv.VectorOfDouble()

    total_volume = 1.0
    total_voxels = 1

    for dim in range(dimension):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        O = random.uniform(0.001, 1e4)
        number_of_voxels.push_back(N)
        image_sizes.push_back(L)
        origin.push_back(O)
        total_volume *= L
        total_voxels *= N


    if dimension == 2:
        im = larcv.ImageMeta2D(projection_id, number_of_voxels, image_sizes)
    elif dimension == 3:
        im = larcv.ImageMeta3D(projection_id, number_of_voxels, image_sizes)
    else:
        assert(False)

    assert(im.n_dims() == dimension)
    assert(im.total_voxels() == total_voxels)

@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_unravel(dimension, execution_number):

    
    if dimension == 2:
        im = larcv.ImageMeta2D()
    elif dimension == 3:
        im = larcv.ImageMeta3D()
    else:
        assert(False)


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

@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_ravel(dimension, execution_number):

    
    if dimension == 2:
        im = larcv.ImageMeta2D()
    elif dimension == 3:
        im = larcv.ImageMeta3D()
    else:
        assert(False)

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
        vec_of_indexes = larcv.VectorOfSizet()
        for dim in dims:
            ind = random.randint(0, dim-1)
            indexes.append(ind)
            vec_of_indexes.push_back(ind)

        # for 
        np_raveled = numpy.ravel_multi_index(indexes, dims)
        im_raveled = im.index(vec_of_indexes)

        for d in range(dimension):
            assert(np_raveled == im_raveled)
