import pytest
import larcv
import numpy

from larcv import data_generator

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


def image_meta_factory(dimension):
    if dimension == 1:
        return larcv.ImageMeta1D()
    elif dimension == 2:
        return larcv.ImageMeta2D()
    elif dimension == 3:
        return larcv.ImageMeta3D()
    elif dimension == 4:
        return larcv.ImageMeta4D()
    else:
        raise Expection("Can't do anything with dimension == ", dimension)


@pytest.mark.parametrize('dimension', [2,3])
@pytest.mark.parametrize('pooling', [larcv.kPoolAverage, larcv.kPoolMax, larcv.kPoolSum])
def test_sparse_tensor_downsample(dimension, pooling):


    # Create image Meta:
    meta = image_meta_factory(dimension)
    meta.set_projection_id(0)
    for dim in range(dimension):
        L = 10.
        N = 128
        meta.set_dimension(dim, L, N)
    
    if dimension == 2:
        st = larcv.SparseTensor2D()
    if dimension == 3:
        st = larcv.SparseTensor3D()
    st.meta(meta)
    

    # Get a set of voxels:
    voxel_set_list = data_generator.build_sparse_tensor(1, n_projections = 1)
    indexes = voxel_set_list[0][0]['indexes']
    values = voxel_set_list[0][0]['values']
    n_voxels = voxel_set_list[0][0]['n_voxels']
    for j in range(n_voxels):
        if pooling == larcv.kPoolMax:
            # Only use positive values for max pooling.  
            # Negative values have different behavior in sparse vs dense
            # max pooling.
            st.emplace(larcv.Voxel(indexes[j], numpy.abs(values[j])), False)
        else:
            st.emplace(larcv.Voxel(indexes[j], values[j]), False)

    # Dense downsampling is tested against skimage
    # Here, test against dense downsampling

    compression = 2

    st_dense = st.to_tensor()
    st_dense_compressed = st_dense.compress(compression, pooling).as_array()

    st_compressed = st.compress(compression, pooling)

    st_compressed_dense = st_compressed.dense()
    print(st_dense.as_array())

    # Do some checks:
    assert numpy.abs( (st_compressed_dense.sum() - st_dense_compressed.sum()) / st_dense_compressed.sum() )< 1e-6
    max_index = numpy.prod(st_compressed_dense.shape)
    for i in range(50):
        index = numpy.random.randint(0, max_index)
        assert numpy.abs(st_compressed_dense.take(index) - st_dense_compressed.take(index) )< 1e-4




