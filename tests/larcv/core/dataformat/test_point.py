import unittest
import numpy
import pytest

from random import Random
random = Random()

N_CHECKS = 1

@pytest.mark.parametrize('dimension', [2,3])
def test_import(dimension):
    import larcv
    if dimension == 2:
        pt = larcv.Point2D()
    elif dimension == 3:
        pt = larcv.Point3D()
    else:
        assert False
