import unittest
import larcv

# Testing Voxel objects:
def test_Voxel_h_Voxel():
    vox1 = larcv.Voxel()
    vox2 = larcv.Voxel(0, 0.0)
    vox3 = larcv.Voxel(10,50.0)

    assert(vox1.id() == larcv.kINVALID_VOXELID)
    assert(vox2.id() == 0)
    assert(vox3.id() == 10)


    assert(vox1.value() == larcv.kINVALID_FLOAT)
    assert(vox2.value() == 0.0)
    assert(vox3.value() == 50.)

def test_Voxel_h_VoxelSet():

    vs = larcv.VoxelSet()
    
    n_voxels = 10
    for i in range(n_voxels):
        vs.emplace(i, i, False)

    vec = vs.as_vector()

    assert(len(vec) == n_voxels) 
    vs.clear_data()

    n_voxels = 11
    for j in range(n_voxels):
        vs.insert(larcv.Voxel(i+j, i+j))

    vs.clear_data()
    n_voxels = 12
    for j in range(n_voxels):
        vs.add(larcv.Voxel(i+j, i+j))


    assert(vs.size() == n_voxels) 


def test_Voxel_h_VoxelSetArray():

    vsa = larcv.VoxelSetArray()
    
    n_voxel_sets = 3
    for i in range(n_voxel_sets):
        n_voxels = 10*(i+1)
        vs = larcv.VoxelSet()
        vs.id(i)
        for j in range(n_voxels):
            vs.emplace(j, j, False)

        vsa.insert(vs)

    assert(vsa.size() == n_voxel_sets)
    assert(vsa.sum() > 0)