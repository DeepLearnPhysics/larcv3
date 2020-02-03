import unittest
from larcv import larcv

from random import Random
random = Random()

def test_import_BatchDataStorageFloat_h():

    larcv.BatchDataStorageFactoryFloat()
    larcv.BatchDataStorageFloat()
    larcv.BatchDataFloat()