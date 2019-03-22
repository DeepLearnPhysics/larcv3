import unittest


# Testing Voxel objects:
def test_Voxel_h_Voxel():
    from larcv import dataformat
    vox1 = dataformat.Voxel()
    vox2 = dataformat.Voxel(0, 0.0)
    vox3 = dataformat.Voxel(10,50.0)

    assert(vox1.id() == dataformat.kINVALID_VOXELID)
    assert(vox2.id() == 0)
    assert(vox3.id() == 10)

    from larcv import base
    assert(vox1.value() == base.kINVALID_FLOAT)
    assert(vox2.value() == 0.0)
    assert(vox3.value() == 50.)

def test_Voxel_h_VoxelSet():
    from larcv import dataformat

    vs = dataformat.VoxelSet()
    
    n_voxels = 10
    for i in range(n_voxels):
        vs.emplace(i, i, False)

    vec = vs.as_vector()

    assert(vec.size() == n_voxels) 
    vs.clear_data()

    n_voxels = 11
    for j in range(n_voxels):
        vs.emplace(dataformat.Voxel(i+j, i+j), True)
    assert(vec.size() == n_voxels) 



def test_Voxel_h_VoxelSetArray():
    from larcv import dataformat

    vsa = dataformat.VoxelSetArray()
    
    n_voxel_sets = 3
    for i in range(n_voxel_sets):
        n_voxels = 10*(i+1)
        vs = dataformat.VoxelSet()
        vs.id(i)
        for j in range(n_voxels):
            vs.emplace(j, j, False)

        vsa.emplace(vs)

    assert(vsa.size() == n_voxel_sets)
    assert(vsa.sum() > 0)