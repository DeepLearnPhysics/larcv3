import unittest
import numpy
import pytest

import larcv

from random import Random
random = Random()

N_CHECKS = 1

def bbox_factory(dimension):
    if dimension == 2:
        return larcv.BBox2D()
    elif dimension == 3:
        return larcv.BBox3D()
    else:
        raise Exception("Can't do anything with dimension == ", dimension)


@pytest.mark.parametrize('dimension', [2,3])
def test_import(dimension):
    
    try:
        bbox = bbox_factory(dimension)
    except:
        assert False

@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_default_constructor(dimension, execution_number):
    
    bbox = bbox_factory(dimension)


    assert(bbox.centroid() == [0.0]*dimension)
    assert(bbox.half_length() == [1.0]*dimension)



@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('execution_number', range(N_CHECKS))
def test_filled_constructor(dimension, execution_number):

    
    centroid    = [random.uniform(-1e4, 1e4) for d in range(dimension)]
    half_length = [random.uniform(-1e4, 1e4) for d in range(dimension)]

    if dimension == 2:
        bbox = larcv.BBox2D(centroid, half_length)
    elif dimension == 3:
        bbox = larcv.BBox3D(centroid, half_length)


    assert(bbox.centroid() == centroid)
    assert(bbox.half_length() == half_length)
