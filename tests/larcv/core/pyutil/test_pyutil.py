import unittest
import pytest

def test_import_pyutil():
    from larcv import pyutil


# def test_vector_conversion():
#     from larcv import pyutil
#     from larcv import dataformat

#     vec = dataformat.VectorOfFloat();
#     for i in range(50):
#         vec.append(i)
#     print(vec)
#     print(type(vec))
#     ndarray = pyutil.as_ndarray_float(vec)

@pytest.mark.parametrize("x_dim", [1,10,25,100])
@pytest.mark.parametrize("y_dim", [1,11,24,4])
def test_image2d_conversion(x_dim, y_dim):
    import numpy
    from larcv import dataformat, pyutil

    # Start with a 2D numpy array:
    numpy_image = numpy.random.random([x_dim, y_dim]).astype("float32")

    # Convert the numpy array to image 2D:
    image2d = pyutil.as_image2d(numpy_image)

    # Convert the image back to numpy array:

    numpy_image_back = pyutil.as_ndarray(image2d)


    # Check the values of the tensor:
    for i in range(x_dim):
        for j in range(y_dim):
            assert(numpy_image[i][j] == numpy_image_back[i][j])
            assert(numpy_image[i][j] == image2d.pixel(i*y_dim + j))



@pytest.mark.parametrize("x_dim", [1,10,25,100])
@pytest.mark.parametrize("y_dim", [1,11,24,4])
def test_image2d_conversion_with_meta(x_dim, y_dim):
    import numpy
    from larcv import dataformat, pyutil


    # Start with a 2D numpy array:
    numpy_image = numpy.random.random([x_dim, y_dim]).astype("float32")

    meta = dataformat.ImageMeta2D()
    meta.set_dimension(0,x_dim, x_dim)
    meta.set_dimension(1,y_dim, y_dim)

    # Convert the numpy array to image 2D:
    image2d = pyutil.as_image2d_meta(numpy_image, meta)

    # Convert the image back to numpy array:

    numpy_image_back = pyutil.as_ndarray(image2d)


    # Check the values of the tensor:
    for i in range(x_dim):
        for j in range(y_dim):
            assert(numpy_image[i][j] == numpy_image_back[i][j])
            assert(numpy_image[i][j] == image2d.pixel(i*y_dim + j))




