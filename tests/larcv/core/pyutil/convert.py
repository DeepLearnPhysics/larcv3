import numpy
from larcv import dataformat, pyutil

# Start with a 2D numpy array:
numpy_image = numpy.array([[1.0,2.0,3.0], [4.0,5.0,6.0], [7.0,8.0,9.0], [3.0, 5.0, 0.0]],dtype="float32")


print(numpy_image)
# Convert the numpy array to image 2D:

meta = dataformat.ImageMeta2D()
meta.set_dimension(0, 3, 3)
meta.set_dimension(0, 4, 4)
print(type(meta))

image2d = pyutil.as_image2d_meta(numpy_image, meta)


for i in range(image2d.size()):
    print(image2d.as_vector()[i])

numpy2 = pyutil.as_ndarray(image2d)

print(numpy2)