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
    print(raw_image.shape)
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
        index = random.randint(0,t.size())
        assert t.pixel(index) == raw_image.take(index)

    nd_t = t.as_array()

    assert (nd_t - raw_image).sum() < 1e-6

    # Verify the tensor and array match by checking elements:
    return True


if __name__ == '__main__':
    test_Tensor(1)
    test_Tensor(2)
    test_Tensor(3)
    test_Tensor(4)
