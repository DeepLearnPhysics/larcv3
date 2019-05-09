import unittest



def test_import_processor_top():
    from larcv import larcv

def test_import_processor():
    from larcv import larcv

    lib = larcv.ProcessDriver()
    lib = larcv.ProcessFactory()

def test_import_processor_base():
    from larcv import larcv

    # Processes base is virtual and should NOT import:
    try:
        lib = larcv.ProcessBase("test")
        assert(False)
    except:
        assert(True)

def test_process_driver():
    from larcv import larcv

    lib = larcv.ProcessDriver("test")
    