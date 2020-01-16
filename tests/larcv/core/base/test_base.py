import unittest


def test_import_base_full():
    import larcv
    lib = larcv.logger()
    lib = larcv.larcv_base()
    lib = larcv.PSet("test")
    lib = larcv.Watch()
    # Config manager is deliberately excluded:
    # lib = base.ConfigManager()