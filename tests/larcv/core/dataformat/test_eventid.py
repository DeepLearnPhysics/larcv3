import unittest
import numpy
import pytest



def test_import_eventID():
    import larcv

    a = larcv.EventID()

    # By Default, it should be invalid on construction:

    assert (a.valid() == False)

    return True

