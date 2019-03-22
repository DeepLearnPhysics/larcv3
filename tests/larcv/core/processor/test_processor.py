import unittest



def test_import_processor_top():
    from larcv import processor

def test_import_processor():
    from larcv import processor

    lib = processor.ProcessDriver()
    lib = processor.ProcessFactory()

def test_import_processor_base():
    from larcv import processor

    # Processes base is virtual and should NOT import:
    try:
        lib = processor.ProcessBase("test")
        assert(False)
    except:
        assert(True)