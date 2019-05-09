import unittest

def test_fake():
    import os
    print(os.environ)
    import larcv
    print(os.environ)
    return False
    
def test_import():
    import larcv
    from larcv import larcv



