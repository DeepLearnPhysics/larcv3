#ifndef __LARCAFFE_IMAGEMETA_CXX__
#define __LARCAFFE_IMAGEMETA_CXX__

#include "ImageMeta.h"
#include "core/base/larbys.h"
#include "core/base/larcv_logger.h"
#include <sstream>
namespace larcv {

/// Default constructor: Does nothing, valid defaults to false
ImageMeta::ImageMeta() { _valid = false; }


/// Constructor with arguments: ndims, dims, voxel_sizes, unit.
ImageMeta::ImageMeta(size_t n_dims, const std::vector<size_t>& number_of_voxels,
                     const std::vector<double>& image_sizes,
                     const std::vector<double>& origin,
                     DistanceUnit_t unit) {
  if ( n_dims != 0 && 
       _number_of_voxels.size() == n_dims &&
       _image_sizes.size() == n_dims
     ){

    // Set the origin to 0 if not set
    if (origin.size() == _n_dims){
      _origin = origin;
    }
    else{
      _origin.resize(n_dims);
    }

    _n_dims           = n_dims;
    _image_sizes      = image_sizes;
    _number_of_voxels = number_of_voxels;
    _unit             = unit;
    _origin           = origin;
    _valid            = true;

  }
  else{
    LARCV_CRITICAL() << "Invalid parameters passed to imagemeta, can not create valid meta." << std::endl;
    throw larbys();
  }

}




size_t ImageMeta::image_size(size_t axis)       const{
  if (_valid && axis >= 0 && axis < _n_dims){
    return _image_sizes.at(axis);
  } 
  else{
    LARCV_CRITICAL() << "Can't return image size of invalid meta." << std::endl;
    throw larbys();
  }
}
size_t ImageMeta::number_of_voxels(size_t axis) const{
  if (_valid && axis >= 0 && axis < _n_dims){
    return _image_sizes.at(axis);
  } 
  else{
    LARCV_CRITICAL() << "Can't return number_of_voxels of invalid meta." << std::endl;
    throw larbys();
  }
}

size_t ImageMeta::total_voxels() const{
  if (_valid){
    size_t i = 1;
    for (auto count : _number_of_voxels) i *= count;
    return i;
  }
  else{
    LARCV_CRITICAL() << "Can't return total voxels of invalid meta." << std::endl;
    throw larbys();
  }
}

std::vector<double> ImageMeta::voxel_dimensions()  const{
  if (_valid){
    std::vector<double> dims; dims.resize(_n_dims);
    for (size_t axis = 0; axis < _n_dims; ++ axis) {
      dims.at(axis) = _image_sizes.at(axis) / _number_of_voxels.at(axis);
    }
    return dims;
  }
  else{
    LARCV_CRITICAL() << "Can't return voxel dimensions of invalid meta." << std::endl;
    throw larbys();
  }
}
double ImageMeta::voxel_dimensions(size_t axis)    const{
  if (_valid && axis >= 0 && axis < _n_dims){
    return _image_sizes.at(axis) / _number_of_voxels.at(axis);
  }
  else{
    LARCV_CRITICAL() << "Can't return voxel dimensions of invalid meta." << std::endl;
    throw larbys();
  }
}


/// Provide 1-D array index dimension from general coordinates
size_t ImageMeta::index(const std::vector<size_t> & coordinate) const{
  // By convention, the conversion from index to coordinate uses the last coordinate as the most frequently
  // changing, and first coordinate as the most commonly changing.

  if (_valid ){

    if (coordinate.size() != _n_dims){
      LARCV_CRITICAL() << "Incomplete coordinate submitted for conversion to index, abort." << std::endl;
      throw larbys();
    }

    size_t index = 0;
    size_t stride = 1;
    for (size_t axis = _n_dims - 1; axis >= 0; -- axis) {
      index += coordinate.at(axis)*stride;
      stride += _number_of_voxels.at(axis);
      // dims.at(axis) = _image_sizes.at(axis) / _number_of_voxels.at(axis);
    }
    return index;
  }
  else{
    LARCV_CRITICAL() << "Can't return voxel dimensions of invalid meta." << std::endl;
    throw larbys();
  }

}

/// Vectorized version for converting all coordinates to index:
/// Coordinates is flattened here as [x0, y0, z0, x1, y1, z1, x2, y2, z2, x3 ....]
void ImageMeta::index( const std::vector<size_t> & coordinates, std::vector<size_t> & output_index) const{


  if (_valid){

    if (coordinates.size() != _n_dims){
      LARCV_CRITICAL() << "Incomplete coordinates submitted for conversion to index, abort." << std::endl;
      throw larbys();
    }

    output_index.clear();
    output_index.resize(coordinates.size());

    #pragma omp parallel for
    for (size_t i = 0; i < coordinates.size(); i ++){
      // size_t index = 0;
      size_t stride = 1;
      for (size_t axis = _n_dims - 1; axis >= 0; -- axis) {
        output_index.at(i) += coordinates.at(axis)*stride;
        stride += _number_of_voxels.at(axis);
        // dims.at(axis) = _image_sizes.at(axis) / _number_of_voxels.at(axis);
      }
    }

    
    return;
  }
  else{
    LARCV_CRITICAL() << "Can't return voxel dimensions of invalid meta." << std::endl;
    throw larbys();
  }

}


/// Convert 1D index to overall coordinates
std::vector<size_t> ImageMeta::coordinates( size_t index) const{
  if (_valid ){

    std::vector<size_t> coordinates;
    coordinates.resize(_n_dims);

    // size_t index = 0;
    for (size_t axis = _n_dims - 1; axis >= 0; -- axis) {
      size_t stride = _number_of_voxels.back();
      coordinates.at(axis) = index % stride;
      index = index / stride;
      // dims.at(axis) = _image_sizes.at(axis) / _number_of_voxels.at(axis);
    }
    
    return coordinates;
  }
  else{
    LARCV_CRITICAL() << "Can't return coordinates of invalid meta." << std::endl;
    throw larbys();
  }

}
  /// Vectorized version for converting all indexes into coordinates
  /// Coordinates is flattened here as [x0, y0, z0, x1, y1, z1, x2, y2, z2, x3 ....]
void ImageMeta::coordinates( const std::vector<size_t> & index,  std::vector<size_t> & output_coordinates) const{

  if (_valid ){

    if (index.size() != _n_dims){
      LARCV_CRITICAL() << "Incomplete index submitted for conversion to coordinates, abort." << std::endl;
      throw larbys();
    }

    output_coordinates.resize(index.size() * _n_dims);

    #pragma omp parallel for
    for (size_t i_index = 0; i_index < index.size(); ++ i_index){
      size_t index_copy = index.at(i_index);
      for (size_t axis = _n_dims - 1; axis >= 0; -- axis) {
        size_t stride = _number_of_voxels.back();
        output_coordinates.at(i_index*_n_dims + axis) = index_copy % stride;
        index_copy = index_copy / stride;
        // dims.at(axis) = _image_sizes.at(axis) / _number_of_voxels.at(axis);
      }

    }
    
    return;
  }
  else{
    LARCV_CRITICAL() << "Can't return coordinates of invalid meta." << std::endl;
    throw larbys();
  }

}



  /// Convert 1D index to overall coordiante along specified axis
size_t ImageMeta::coordinate(size_t index, size_t axis) const{
  if (_valid && axis >= 0 && axis < _n_dims){

    // size_t index = 0;
    for (size_t loop_axis = _n_dims - 1; loop_axis >= 0; -- loop_axis) {
      size_t stride = _number_of_voxels.back();
      size_t coordinate = index % stride;
      index = index / stride;
      if (loop_axis == axis){
        return coordinate;
      }

    }
    return kINVALID_INDEX;
    
  }
  else{
    LARCV_CRITICAL() << "Can't return coordinates of invalid meta." << std::endl;
    throw larbys();
  }
}


  /// There is no vectorized version of the single axis coordinate, open an issue if you need it.


  /// Provide absolute coordinate of the center of a specified pixel index
std::vector<double> ImageMeta::position(size_t index) const{
  if (_valid ){


    std::vector<size_t> coords = coordinates(index);    
    return position(coords);
  }
  else{
    LARCV_CRITICAL() << "Can't return positions of invalid meta." << std::endl;
    throw larbys();
  }

}


  /// Provide absolute coordinate of the center of a specified pixel (row,col)
std::vector<double> ImageMeta::position(const std::vector<size_t> & coordinates) const{
  if (_valid ){


    std::vector<double> positions;
    positions.resize(_n_dims);

    // size_t index = 0;
    for (size_t axis = _n_dims - 1; axis >= 0; -- axis) {
      // Calculate the voxel size:
      double voxel_size = (_image_sizes.at(axis) / _number_of_voxels.at(axis) );
      positions.at(axis) = coordinates.at(axis) * voxel_size;
      /// Measure to the center of each voxel
      positions.at(axis) += 0.5*voxel_size + _origin.at(axis); 
    }
    
    return positions;
  }
  else{
    LARCV_CRITICAL() << "Can't return positions of invalid meta." << std::endl;
    throw larbys();
  }
}

/// Same as above, but restricted to a single axis
double ImageMeta::position(size_t index, size_t axis) const{
  std::vector<size_t> coords = coordinates(index);    
  return position(coords).at(axis);
}

/// Same as above, but restricted to a single axis
double ImageMeta::position(const std::vector<size_t> & coordinates, size_t axis) const{
  return position(coordinates).at(axis);
}


/// Provide the minimum and maximum real space values of the image.
std::vector<double> ImageMeta::min() const{
  if (_valid){
    return _origin;
  }
  else{
    LARCV_CRITICAL() << "Can't return min of invalid meta." << std::endl;
    throw larbys();
  }
}
std::vector<double> ImageMeta::max() const{
  if (_valid){
    std::vector<double> max;
    max.resize(_n_dims);
    for (size_t axis = 0; axis < _n_dims; ++ axis){
      max.at(axis) = _origin.at(axis) + _image_sizes.at(axis);
    } 
    return max;
  }
  else{
    LARCV_CRITICAL() << "Can't return min of invalid meta." << std::endl;
    throw larbys();
  }
}

double ImageMeta::min(size_t axis) const{
  return min().at(axis);
}
double ImageMeta::max(size_t axis) const{
  return max().at(axis);
}


//Provide a conversion from a real position to an index or coordinate
size_t ImageMeta::position_to_index(const std::vector<double> & position ) const{
  std::vector<size_t> coords = position_to_coordinate(position);
  return index(coords);
}

std::vector<size_t> ImageMeta::position_to_coordinate(const std::vector<double> & position) const{
  if (_valid && position.size() == _n_dims){



    std::vector<size_t> coordinates;
    coordinates.resize(_n_dims);

    std::vector<double> dims = voxel_dimensions();
    
    for (size_t axis = 0; axis < _n_dims; axis ++){
      double relative_position = position.at(axis) - _origin.at(axis);
      if (relative_position < 0.0 || relative_position >= _image_sizes.at(axis)){
        coordinates.at(axis) = kINVALID_INDEX;
      }
      coordinates.at(axis) = (size_t) relative_position / dims.at(axis);
    }
    return coordinates;
  }
  else{
    LARCV_CRITICAL() << "Can't return position from coordinate of invalid meta." << std::endl;
    throw larbys();
  }
}


/// Map just one position to just one coordinate
size_t ImageMeta::position_to_coordinate(double position, size_t axis) const{
  if (_valid && axis >= 0 && axis < _n_dims){


    double relative_position = position - _origin.at(axis);
    if (relative_position < 0.0 || relative_position >= _image_sizes.at(axis)){
      return kINVALID_INDEX;
    }
    else{ 
      double voxel_dim = voxel_dimensions(axis);
      return  (size_t) relative_position / voxel_dim;
    }
    
  }
  else{
    LARCV_CRITICAL() << "Can't return position from coordinate of invalid meta." << std::endl;
    throw larbys();
  }
}


















/*
 *  leftover from old implementation that needs to be written.
ImageMeta ImageMeta::overlap(const ImageMeta& meta) const
{
  auto box = BBox2D::overlap((BBox2D)meta);

  return ImageMeta(box,
                   box.height() / pixel_height(),
                   box.width() / pixel_width(),
                   _unit);
}

ImageMeta ImageMeta::inclusive(const ImageMeta& meta) const
{
  auto box = BBox2D::inclusive((BBox2D)meta);

  return ImageMeta(box,
                   box.height() / pixel_height(),
                   box.width() / pixel_width(),
                   _unit);
}

std::string ImageMeta::dump() const
{
  std::stringstream ss;
  ss << "ProjectionID " << id() << " (rows,cols) = (" << _row_count << "," <<
  _col_count
     << ") ... Distance Unit: " << (int)(this-> unit())
     << " ... Left Bottom => Right Top " << ((BBox2D*)(this))->dump();
  return ss.str();
}
*/

}  // namespace larcv

#endif
