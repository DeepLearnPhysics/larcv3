import pytest
import larcv
import numpy

from random import Random
random = Random()


@pytest.mark.parametrize('dimension', [1, 2, 3, 4])
def test_Tensor(dimension):

    # Create a numpy array of the specified dimension with random values:


    shape = [] 
    for dim in range(dimension):
        if dimension < 3:
            shape.append(random.randint(1, 1e3))
        else:
            shape.append(random.randint(1, 20))
                
    raw_image = numpy.random.random(shape).astype("float32")

    # Create a Tensor from the array:
    if dimension == 1:
        t = larcv.Tensor1D(raw_image)
    elif dimension == 2:
        t = larcv.Tensor2D(raw_image)
    elif dimension == 3:
        t = larcv.Tensor3D(raw_image)
    elif dimension == 4:
        t = larcv.Tensor4D(raw_image)

    assert t.size() == numpy.prod(shape)

    for i in range(20):
        index = random.randint(0,t.size() - 1)
        assert t.pixel(index) == raw_image.take(index)

    nd_t = t.as_array()

    assert (nd_t - raw_image).sum() < 1e-6

    # Verify the tensor and array match by checking elements:
    return True

@pytest.mark.parametrize('dimension', [1, 2, 3, 4])
@pytest.mark.parametrize('pooling', [larcv.kPoolAverage, larcv.kPoolMax, larcv.kPoolSum])
def test_tensor_compression(dimension, pooling):
    
    try:
        from skimage.measure import block_reduce
    except:
        pytest.skip("Could not import skimage")

    shape = [] 
    for dim in range(dimension):
        shape.append(64)
                
    raw_image = numpy.random.random(shape).astype("float32")
    # Create a Tensor from the array:
    if dimension == 1:
        t = larcv.Tensor1D(raw_image)
    elif dimension == 2:
        t = larcv.Tensor2D(raw_image)
    elif dimension == 3:
        t = larcv.Tensor3D(raw_image)
    elif dimension == 4:
        t = larcv.Tensor4D(raw_image)

    # Pooling in numpy is not trivial.  So instead of checking every element,
    # We will check against random elements.

    compression = 2

    t_compressed = t.compress(compression, pooling)

    compressed_numpy = t_compressed.as_array()

    # Use skimage to compress the original:
    kernel = (compression,) * dimension
    if pooling == larcv.kPoolAverage:
        sk_reduced = block_reduce(raw_image, kernel,func=numpy.mean)
    elif pooling == larcv.kPoolMax:
        sk_reduced = block_reduce(raw_image, kernel,func=numpy.max)
    elif pooling == larcv.kPoolSum:
        sk_reduced = block_reduce(raw_image, kernel,func=numpy.sum)

    # Do some checks:
    assert numpy.abs( (compressed_numpy.sum() - sk_reduced.sum()) / sk_reduced.sum() )< 1e-6
    max_index = numpy.prod(compressed_numpy.shape)
    for i in range(50):
        index = numpy.random.randint(0, max_index)
        assert numpy.abs(compressed_numpy.take(index) - sk_reduced.take(index) )< 1e-4


if __name__ == '__main__':
    test_Tensor(1)
    test_Tensor(2)
    test_Tensor(3)
    test_Tensor(4)
