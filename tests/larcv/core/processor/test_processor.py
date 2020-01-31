import unittest



def test_import_processor_top():
    import larcv

def test_import_processor():
    import larcv

    lib = larcv.ProcessDriver()
    # lib = larcv.ProcessFactory()

def test_import_processor_base():
    import larcv

    # Processes base is virtual and should NOT import:
    try:
        lib = larcv.ProcessBase("test")
        assert(False)
    except:
        assert(True)

def test_process_driver():
    import larcv

    lib = larcv.ProcessDriver("test")
    