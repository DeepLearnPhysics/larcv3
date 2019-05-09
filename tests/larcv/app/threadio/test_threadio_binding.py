import unittest
from larcv import larcv

from random import Random
random = Random()

# Testing import of Point.h objects:
def test_import_BatchDataStorageFactory_h():

    bd = larcv.BatchDataStorageFactory()
