import unittest
import numpy
import pytest

from random import Random
random = Random()

def test_import():
    from larcv import dataformat
    im = dataformat.ImageMeta()

@pytest.mark.parametrize('execution_number', range(10))
def test_default_constructor(execution_number):
    from larcv import dataformat
    im = dataformat.ImageMeta()

    n_dims = random.randint(1,4)

    total_volume = 1.0
    total_voxels = 1

    for dim in range(n_dims):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        O = random.uniform(0.001, 1e4)
        im.add_dimension(L, N, O)
        total_volume *= L
        total_voxels *= N

    assert(im.n_dims() == n_dims)
    assert(im.total_voxels() == total_voxels)



@pytest.mark.parametrize('execution_number', range(10))
def test_filled_constructor(execution_number):

    from larcv import dataformat

    n_dims = random.randint(1,4)
    projection_id = random.randint(0,10)

    number_of_voxels = dataformat.VectorOfSizet()
    image_sizes = dataformat.VectorOfDouble()
    origin = dataformat.VectorOfDouble()

    total_volume = 1.0
    total_voxels = 1

    for dim in range(n_dims):
        L = random.uniform(0.001, 1e4)
        N = random.randint(1, 2e4)
        O = random.uniform(0.001, 1e4)
        number_of_voxels.push_back(N)
        image_sizes.push_back(L)
        origin.push_back(O)
        total_volume *= L
        total_voxels *= N

    im = dataformat.ImageMeta(n_dims, projection_id, number_of_voxels, image_sizes)

    assert(im.n_dims() == n_dims)
    assert(im.total_voxels() == total_voxels)
