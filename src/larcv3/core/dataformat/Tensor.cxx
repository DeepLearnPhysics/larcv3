#ifndef __LARCV3DATAFORMAT_TENSOR_CXX__
#define __LARCV3DATAFORMAT_TENSOR_CXX__


#include "larcv3/core/base/larbys.h"
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/dataformat/Tensor.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
namespace larcv3 {

  template<size_t dimension>
  Tensor<dimension>::Tensor(const std::vector<size_t> & dims)
   : _meta()
  {
    for(size_t i = 0; i < dims.size(); i ++) _meta.set_dimension(i, 1.0, dims[i]);
    _img.resize(_meta.total_voxels());
  }

  template<size_t dimension>
  Tensor<dimension>::Tensor(const ImageMeta<dimension>& meta)
    : _img(meta.total_voxels(), 0.)
    , _meta(meta)
  {}

  template<size_t dimension>
  Tensor<dimension>::Tensor(const ImageMeta<dimension>& meta, const std::vector<float>& img)
    : _img(img)
    , _meta(meta)
  { if (img.size() != meta.total_voxels()) throw larbys("Inconsistent dimensions!"); }

  template<size_t dimension>
  Tensor<dimension>::Tensor(const Tensor& rhs)
    : _img(rhs._img)
    , _meta(rhs._meta)
  {}

  template<size_t dimension>
  void Tensor<dimension>::reset(const ImageMeta<dimension>& meta)
  {
    _meta = meta;
    if (_img.size() != _meta.total_voxels()) _img.resize(_meta.total_voxels());
    paint(0.);
  }

  /// from numpy ctor
  template<size_t dimension>
  Tensor<dimension>::Tensor(pybind11::array_t<float>  pyarray){

    auto buffer = pyarray.request();


    // First, we do checks that this is an acceptable dimension:
    if (buffer.ndim != dimension){
      LARCV_ERROR() << "ERROR: cannot convert array of dimension " << buffer.ndim 
                    << " to " << std::to_string(dimension) << "D Tensor\n";
      throw larbys();
    }

    // With that satisfied, create the meta object:
    for (size_t dim = 0; dim < dimension; ++dim)
      _meta.set_dimension(dim, (double)(buffer.shape[dim]), (double)(buffer.shape[dim]));
    

    // Now, we copy the data from numpy into our own buffer:
    _img.resize(_meta.total_voxels());

    auto ptr = static_cast<float *>(buffer.ptr);

    for (size_t i = 0; i < _meta.total_voxels(); ++i) {
      _img[i] = ptr[i];
    }

  }

  // Return a numpy array of this object (no copy by default)
  template<size_t dimension>
  pybind11::array_t<float> Tensor<dimension>::as_array(){
    // Cast the dimensions to std::array:
    std::array<size_t, dimension> dimensions;
    for (size_t i = 0; i < dimension; ++i) dimensions[i] = _meta.number_of_voxels(i);
    return pybind11::array_t<float>(
        // _meta.number_of_voxels()[0],
        dimensions,
        {},
        &(_img[0])
      );
  }
  

  template<size_t dimension>
  void Tensor<dimension>::clear() {
    _img.clear();
    _img.resize(1, 0);
    _meta = ImageMeta<dimension>();
    //std::cout << "[Image2D (" << this << ")] Cleared image2D memory " << std::endl;
  }

  template<size_t dimension>
  void Tensor<dimension>::clear_data() { for (auto& v : _img) v = 0.; }

  template<size_t dimension>
  void Tensor<dimension>::set_pixel( size_t index, float value ) {
    if ( index >= _img.size() ) throw larbys("Out-of-bound pixel set request!");
    _img[ index ] = value;
  }

  template<size_t dimension>
  void Tensor<dimension>::set_pixel( const std::vector<size_t> & coords, float value ) {
    size_t idx = _meta.index(coords);
    if ( idx >= _meta.total_voxels() )
      throw larbys("Out-of-bound pixel set request!");
    _img[ idx ] = value;
  }

  template<size_t dimension>
  void Tensor<dimension>::paint(float value)
  { for (auto& v : _img) v = value; }

  template<size_t dimension>
  void Tensor<dimension>::threshold(float thresh, bool lower)
  { for (auto& v : _img) if ( (lower && v < thresh) || (!lower && v > thresh ) ) v = thresh; }

  template<size_t dimension>
  void Tensor<dimension>::binarize(float thresh, float lower_overwrite, float upper_overwrite)
  { for (auto& v : _img) v = (v <= thresh ? lower_overwrite : upper_overwrite); }

  template<size_t dimension>
  float Tensor<dimension>::pixel(const std::vector<size_t> & coords) const{
    // Get the access index:
    return _img[_meta.index(coords)]; 
  }
  template<size_t dimension>
  float Tensor<dimension>::pixel(size_t index) const{
    return _img.at(index); 

  }

  template<size_t dimension>
  void Tensor<dimension>::copy(const std::vector<size_t> & coords, const float* src, size_t num_pixel)
  {
    const size_t idx = _meta.index(coords);
    if (idx + num_pixel - 1 >= _img.size()) throw larbys("memcpy size exceeds allocated memory!");
    memcpy(&(_img[idx]), src, num_pixel * sizeof(float));
  }

  template<size_t dimension>
  void Tensor<dimension>::copy(const std::vector<size_t> & coords, const std::vector<float>& src, size_t num_pixel)
  {
    if (!num_pixel)
      this->copy(coords, (float*)(&(src[0])), src.size());
    else if (num_pixel <= src.size())
      this->copy(coords, (float*)&src[0], num_pixel);
    else
      throw larbys("Not enough pixel in source!");
  }

  template<size_t dimension>
  void Tensor<dimension>::copy(const std::vector<size_t> & coords, const short* src, size_t num_pixel)
  {
    const size_t idx = _meta.index(coords);
    if (idx + num_pixel - 1 >= _img.size()) throw larbys("memcpy size exceeds allocated memory!");
    for (size_t i = 0; i < num_pixel; ++i) _img[idx + i] = src[idx];
  }

  template<size_t dimension>
  void Tensor<dimension>::copy(const std::vector<size_t> & coords, const std::vector<short>& src, size_t num_pixel)
  {
    if (!num_pixel)
      this->copy(coords, (short*)(&(src[0])), src.size());
    else if (num_pixel <= src.size())
      this->copy(coords, (short*)&src[0], num_pixel);
    else
      throw larbys("Not enough pixel in source!");
  }

  template<size_t dimension>
  void Tensor<dimension>::reverse_copy(const std::vector<size_t> & coords, const std::vector<float>& src, size_t nskip, size_t num_pixel)
  {
    size_t idx = 0;
    try {
      idx = _meta.index(coords);
    } catch (const larbys& err) {
      std::cout << "Exception caught @ " << __FUNCTION__ << std::endl
                << "Image2Tensor<dimension>D ... fill coords[0]: " << coords[0] << " => " << (coords[0] + num_pixel - 1) << std::endl
                << "Tensor<dimension> ... orig idx: " << nskip << " => " << nskip + num_pixel - 1 << std::endl
                << "Re-throwing exception..." << std::endl;
      throw err;
    }
    if (!num_pixel) num_pixel = src.size() - nskip;
    if ( (idx + 1) < num_pixel ) num_pixel = idx + 1;
    for (size_t i = 0; i < num_pixel; ++i) { _img[idx + i] = src[nskip + num_pixel - i - 1]; }
  }

  template<size_t dimension>
  void Tensor<dimension>::reverse_copy(const std::vector<size_t> & coords, const std::vector<short>& src, size_t nskip, size_t num_pixel)
  {
    size_t idx = 0;
    try {
      idx = _meta.index(coords);
    } catch (const larbys& err) {
      std::cout << "Exception caught @ " << __FUNCTION__ << std::endl
                << "Tensor<dimension> ... fill coords[0]: " << coords[0] << " => " << (coords[0] + num_pixel - 1) << std::endl
                << "Tensor<dimension> ... orig idx: " << nskip << " => " << nskip + num_pixel - 1 << std::endl
                << "Re-throwing exception..." << std::endl;
      throw err;
    }
    if (!num_pixel) num_pixel = src.size() - nskip;
    if ( (idx + 1) < num_pixel ) num_pixel = idx + 1;
    for (size_t i = 0; i < num_pixel; ++i) { _img[idx + i] = (float)(src[nskip + num_pixel - i - 1]); }
  }

  
  template<size_t dimension>
  std::vector<float>&& Tensor<dimension>::move()
  { return std::move(_img); }

  template<size_t dimension>
  void Tensor<dimension>::move(std::vector<float>&& data)
  { _img = std::move(data); }

  template<size_t dimension>
  Tensor<dimension>& Tensor<dimension>::operator+=(const std::vector<float>& rhs)
  {
    if (rhs.size() != _img.size()) throw larbys("Cannot call += uniry operator w/ incompatible size!");
    for (size_t i = 0; i < _img.size(); ++i) _img[i] += rhs[i];
    return (*this);
  }

  template<size_t dimension>
  Tensor<dimension>& Tensor<dimension>::operator+=(const larcv3::Tensor<dimension>& rhs)
  {
    if (rhs.size() != _img.size()) throw larbys("Cannot call += uniry operator w/ incompatible size!");
    for(size_t index = 0; index < meta().total_voxels(); index ++ ){
      set_pixel(index, pixel(index) + rhs.pixel(index));
    }
    return (*this);
  }

  template<size_t dimension>
  Tensor<dimension>& Tensor<dimension>::operator-=(const std::vector<float>& rhs)
  {
    if (rhs.size() != _img.size()) throw larbys("Cannot call += uniry operator w/ incompatible size!");
    for (size_t i = 0; i < _img.size(); ++i) _img[i] -= rhs[i];
    return (*this);
  }



  template<size_t dimension>
  void Tensor<dimension>::eltwise(const std::vector<float>& arr, bool allow_longer) {
    // check multiplication is valid
    if ( !( (allow_longer && _img.size() <= arr.size()) || arr.size() == _img.size() ) ) {
      char oops[500];
      sprintf( oops, "Image2D element-wise multiplication not valid. LHS size = %zu, while argument size = %zu",
               _img.size(), arr.size());
      throw larbys(oops);
    }
    // Element-wise multiplication: do random access as dimension has already been checked
    for (size_t i = 0; i < _img.size(); ++i)
      _img[i] *= arr[i];
  }

  template<size_t dimension>
  void Tensor<dimension>::eltwise( const Tensor<dimension>& rhs ) {
    // check multiplication is valid
    auto const& meta = rhs.meta();
    if (meta.total_voxels() != _meta.total_voxels() ) {
      char oops[500];
      sprintf( oops, "Tensor<dimension> element-wise multiplication not valid. LHS n_voxels (%zu) != RHS n_voxels (%zu)",
               _meta.total_voxels(), meta.total_voxels());
      throw larbys(oops);
    }

    eltwise(rhs.as_vector(), false);
  }


template<size_t dimension>
Tensor<dimension> Tensor<dimension>::compress(
  std::array<size_t, dimension> compression, PoolType_t pool_type) const
{

  // First, compress the meta:
  ImageMeta<dimension> compressed_meta = this->_meta.compress(compression);

  float unfilled_value(0.0);
  // default fill value depends on pooling type:
  if (pool_type == larcv3::kPoolMax)
    unfilled_value = - std::numeric_limits< float >::max();

  // Create an output tensor:
  Tensor<dimension> output(compressed_meta);
  std::vector<float> output_data;
  output_data.resize(compressed_meta.total_voxels(), unfilled_value);

  // Loop over the pixels, find the position in the new tensor, and add it.
  for (size_t index = 0; index < _img.size() ;  index ++  ){
    // First, get the old coordinates of this voxel:
    auto coordinates = this->_meta.coordinates(index);
    for (size_t d = 0; d < dimension; d ++) coordinates[d] = size_t(coordinates[d] / compression[d]);
    size_t new_index = compressed_meta.index(coordinates);

    // Add the new voxel to the new set:
    if ( pool_type == larcv3::kPoolMax){
      if (output_data[new_index] < _img[index]){
        // Replace only if this new value is larger than the old
        output_data[new_index] = _img[index];
      } 
      
    }
    else {
      output_data[new_index] += _img[index];
    }
  }

  output.move(std::move(output_data));

  // Correct the output values by the total ratio of compression if averaging:
  if (pool_type == larcv3::kPoolAverage){
    float ratio = 1.0;
    for (size_t d = 0; d < dimension; d ++) ratio *= compression[d];
    output /= ratio;
  }

  return output;

}

template<size_t dimension>
Tensor<dimension> Tensor<dimension>::compress(
  size_t compression, PoolType_t pool_type) const
{

  std::array<size_t, dimension> comp;
  for (size_t i = 0; i < dimension; ++i ) comp[i] = compression;

  return this->compress(comp, pool_type);


}



template class Tensor<1>;
template class Tensor<2>;
template class Tensor<3>;
template class Tensor<4>;

template<> std::string as_string<Tensor<1>>() {return "Tensor1D";}
template<> std::string as_string<Tensor<2>>() {return "Tensor2D";}
template<> std::string as_string<Tensor<3>>() {return "Tensor3D";}
template<> std::string as_string<Tensor<4>>() {return "Tensor4D";}
}

#endif

#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

template <size_t dimension>
void init_tensor_base(pybind11::module m){

  using Class = larcv3::Tensor<dimension>;
  pybind11::class_<Class> tensor(m, larcv3::as_string<Class>().c_str());

  tensor.doc() = R"pbdoc(
  Tensor
  ******************************

  A Tensor is a densely stored, memory-contiguous representation of image-like data.
  Tensors have ImageMeta associated to them to provide external context for the image.
  In this case, the coordinates of a pixel (X/Y/Z ...) map to a 1D index through the ImageMeta
  interface, and are identical to the indexing required in various array interfaces.

  For example, a set of coordinates in 2D (for example) x and y might map to an index through
  the ImageMeta object.  In that case, the following techniques should provide identical data:

  >>> # Access directly:
  >>> tensor.pixel((x,y)) # use the pixel method of the tensor.

  >>> # Same as numpy:
  >>> tensor.as_array[x][y] # convert this tensor to numpy and access with numpy's array indexing.
  
  >>> # Using indexes:
  >>> index = tensor.meta().index((x,y)) # get the index from the image meta
  >>> tensor.pixel(index) # access the pixel via the index

  >>> # Using numpy methods:
  >>> dims = tensor.meta.number_of_voxels()
  >>> index = numpy.ravel_multi_index((x,y), dims)
  >>> tensor.pixel(index) # use numpy constructed index to get the data directly


  The availablitity of both interfaces is to serve as a bridge between ``numpy.ndarray`` and
  SparseTensors, which have only the ``index``-style access (through Voxel IDs).  

  )pbdoc";

  tensor.def(pybind11::init<>());
  tensor.def(pybind11::init<const std::vector<size_t> &> ());
  tensor.def(pybind11::init<const larcv3::ImageMeta<dimension>& > ());
  tensor.def(pybind11::init<const larcv3::ImageMeta<dimension>&, const std::vector<float>&> ());
  tensor.def(pybind11::init<const larcv3::Tensor<dimension>&> ());
  tensor.def(pybind11::init<pybind11::array_t<float, pybind11::array::c_style>> ());

  tensor.def("reset",                                      &Class::reset,
    "Reset the Tensor to an empty state.");
  tensor.def("size",                                       &Class::size,
   "Return the total number of points in the data array." );
  tensor.def("pixel",
    (float (Class::*)(const std::vector<size_t> & ) const)(&Class::pixel),
    pybind11::arg("coordinates"),
    "Access a pixel via a list of coordinates.");
  tensor.def("pixel", 
    (float (Class::*)(size_t ) const)(                     &Class::pixel),
    pybind11::arg("index"),
    "Access the pixel at a specified index.");
  tensor.def("meta",                                       &Class::meta,
    "Get the meta of this tensor.");
  tensor.def("as_vector",                                  &Class::as_vector,
    "Get the single-dimensioned array of memory-contiguous data.");
  tensor.def("set_pixel",
    (void (Class::*)(const std::vector<size_t> &, float ))(&Class::set_pixel),
    pybind11::arg("coordinates"), pybind11::arg("value"),
    "Set an individual pixel's ``value`` at ``coordinates``. ");
  tensor.def("set_pixel", 
    (void (Class::*)( size_t, float  ))(                   &Class::set_pixel),
    pybind11::arg("index"), pybind11::arg("value"),
    "Set an individual pixel's ``value`` at ``index``. "
    );
  tensor.def("paint",                                      &Class::paint,
    pybind11::arg("value"),
    "Set ALL pixels to the specified ``value``.");
  tensor.def("set_projection_id",                          &Class::set_projection_id,
    pybind11::arg("id"),
    "Set the projection id of the meta for this tensor to ``id``.");

  tensor.def("threshold",                                  &Class::threshold,
    pybind11::arg("threshold"), pybind11::arg("lower")=true,
    "If ``lower`` is true, this is minimum threshold.  Otherwise, this is a maximum threshold.");

  tensor.def("binarize",                                   &Class::binarize,
    pybind11::arg("threshold"), pybind11::arg("lower"), pybind11::arg("upper"),
    "Replace all pixels with either the ``lower`` or ``upper`` value, based on ``threshold``.");
  tensor.def("clear_data",                                 &Class::clear_data,
    "Set the image data to 0."
    );
  tensor.def("compress",
    (Class (Class::*)(std::array<size_t, dimension> compression, larcv3::PoolType_t)const)(&Class::compress),
      pybind11::arg("compression"), pybind11::arg("pooltype"),
      "Return a new tensor that is compressed/downsampled according to the PoolType, with ``compression`` unique per axis.");


  tensor.def("compress", 
    (Class (Class::*)( size_t, larcv3::PoolType_t ) const)( &Class::compress),
    pybind11::arg("compression"), pybind11::arg("pool_type"),
    "Return a new tensor that is compressed/downsampled equally along all axes."
    );

  tensor.def(pybind11::self += float());
  tensor.def(pybind11::self + float());
  tensor.def(pybind11::self -= float());
  tensor.def(pybind11::self - float());
  tensor.def(pybind11::self *= float());
  tensor.def(pybind11::self * float());
  tensor.def(pybind11::self /= float());
  tensor.def(pybind11::self / float());
  // tensor.def(pybind11::self += const std::vector<float>&);
  // tensor.def(pybind11::self -= const std::vector<float>&);
  tensor.def(pybind11::self += pybind11::self);

  tensor.def("eltwise",
    (void (Class::*)(const Class& rhs ))(&Class::eltwise),
    pybind11::arg("other"),
    "Element wise multiplication with another array.  Equivalent to dot product in 1D.");
  tensor.def("eltwise", 
    (void (Class::*)( const std::vector<float>&, bool  ))(&Class::eltwise),
    pybind11::arg("other"), pybind11::arg("allow_longer") = false,
    "Element wise multiplication with another array.  Equivalent to dot product in 1D.");

  tensor.def("as_array", &Class::as_array,
    "Return the numpy view of this tensor.");
  

}



void init_tensor(pybind11::module m){
  init_tensor_base<1>(m);
  init_tensor_base<2>(m);
  init_tensor_base<3>(m);
  init_tensor_base<4>(m);
}
