import unittest



def test_import_base_top():
    from larcv import base

def test_import_base_full():
    from larcv import base
    lib = base.logger()
    lib = base.larcv_base()
    lib = base.PSet("test")
    lib = base.Watch()
    # Config manager is deliberately excluded:
    # lib = base.ConfigManager()