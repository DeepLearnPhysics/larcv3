import unittest



def test_larbys():
    from larcv import base
    lib = base.larbys()
    # Config manager is deliberately excluded:
    # lib = base.ConfigManager()