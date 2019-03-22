import unittest

def test_import_dataformat_top():
    from larcv import dataformat

# Testing import of Point.h objects:
def test_import_Point_h():
    from larcv import dataformat
    lib = dataformat.Point2D()
    lib = dataformat.Point3D()

# Testing import of Vertex.h objects:
def test_import_Vertex_h():
    from larcv import dataformat
    vert1 = dataformat.Vertex()
    vert2 = dataformat.Vertex(1,2,3,4)
    assert(vert1 != vert2)
    assert(vert1 < vert2)
    vert2.reset()
    assert(vert1 == vert2)

# Testing import of Particle.h objects:
def test_import_Particle_h():
    from larcv import dataformat
    part1 = dataformat.Particle()

# Testing import of Voxel.h objects:
def test_import_Voxel_h_Voxel():
    from larcv import dataformat

    v = dataformat.Voxel()

def test_import_Voxel_h_VoxelSet():
    from larcv import dataformat

    vs = dataformat.VoxelSet()

def test_import_Voxel_h_VoxelSetArray():
    from larcv import dataformat

    vsa = dataformat.VoxelSetArray()
    
def test_import_ImageMeta_h():
    from larcv import dataformat

    im = dataformat.ImageMeta()

def test_import_EventID_h():
    from larcv import dataformat

    im = dataformat.EventID()

# Event Base is abstract and therefore should NOT import:
def test_import_EventBase_h():
    from larcv import dataformat

    try:
        eb = dataformat.EventBase()
        assert(False)
    except:
        assert(True)

def test_import_DataProductFactory_h():
    from larcv import dataformat
    # dfb = dataformat.DataProductFactoryBase()
    df = dataformat.DataProductFactory()

def test_import_IOManager_h():
    from larcv import dataformat
    io = dataformat.IOManager()

def test_import_EventParticle_h():
    from larcv import dataformat
    ep = dataformat.EventParticle()
