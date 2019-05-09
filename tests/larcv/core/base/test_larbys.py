import unittest



def test_larbys():
    from larcv import larcv
    lib = larcv.larbys()
    # Config manager is deliberately excluded:
    # lib = base.ConfigManager()