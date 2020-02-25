import unittest



def test_larbys():
    import larcv
    lib = larcv.larbys()
    # Config manager is deliberately excluded:
    # lib = base.ConfigManager()

def test_larbys_exception():
    import larcv
    # Creating a PSet object without a name will cause an exception.
    # We can catch that and verify we're getting a larbys exception:
    try:
        pset = larcv.PSet()
        return False
    except:
        return True