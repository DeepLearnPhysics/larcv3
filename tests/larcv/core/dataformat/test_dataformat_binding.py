import unittest

import larcv

from random import Random
random = Random()


# Testing import of Point.h objects:
def test_import_Point_h():
    pt = larcv.Point2D()
    pt = larcv.Point3D()

def test_import_BBox_h():
    bb = larcv.BBox2D()
    bb = larcv.BBox3D()


# Testing import of Vertex.h objects:
def test_import_Vertex_h():
    vert1 = larcv.Vertex()
    vert2 = larcv.Vertex(1,2,3,4)
    assert(vert1 != vert2)
    assert(vert1 < vert2)
    vert2.reset()
    assert(vert1 == vert2)

# Testing import of Particle.h objects:
def test_import_Particle_h():
    part1 = larcv.Particle()

# Testing import of Voxel.h objects:
def test_import_Voxel_h_Voxel():

    v = larcv.Voxel()

def test_import_Voxel_h_VoxelSet():

    vs = larcv.VoxelSet()

def test_import_Voxel_h_VoxelSetArray():

    vsa = larcv.VoxelSetArray()
    
def test_import_ImageMeta_h():

    im = larcv.ImageMeta2D()
    im = larcv.ImageMeta3D()

def test_import_EventID_h():

    im = larcv.EventID()

# Event Base is abstract and therefore should NOT import:
def test_import_EventBase_h():

    try:
        eb = larcv.EventBase()
        assert(False)
    except:
        assert(True)


# There is no need to wrap data product factory, so no test:
# def test_import_DataProductFactory_h():
#     # dfb = larcv.DataProductFactoryBase()
#     df = larcv.DataProductFactory()

def test_import_IOManager_h():
    io = larcv.IOManager()

def test_import_EventParticle_h():
    ep = larcv.EventParticle()


# def test_vector_double():
#     vec = larcv.VectorOfDouble()
#     vec.push_back(random.uniform(-1e5, 1e5))  

# def test_vector_sizet():
#     vec = larcv.VectorOfSizet()
#     vec.push_back(random.randint(1, 2e4))  

