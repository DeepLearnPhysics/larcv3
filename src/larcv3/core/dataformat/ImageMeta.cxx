#ifndef __LARCV3DATAFORMAT_IMAGEMETA_CXX__
#define __LARCV3DATAFORMAT_IMAGEMETA_CXX__

#include "larcv3/core/dataformat/ImageMeta.h"
#include "larcv3/core/base/larbys.h"
#include "larcv3/core/base/larcv_logger.h"
#include <sstream>
namespace larcv3 {

/// Default constructor: Does nothing, valid defaults to false
template<size_t dimension>
ImageMeta<dimension>::ImageMeta() : 
 _valid(false)
, _projection_id(0)
, _unit(kUnitUnknown)
{
  for(size_t i = 0; i < dimension; i++){
    _image_sizes[i] = 0;
    _number_of_voxels[i] = 0;
    _origin[i] = 0;
  }
 }


/// Constructor with arguments: ndims, dims, voxel_sizes, unit.
template<size_t dimension>
ImageMeta<dimension>::ImageMeta(size_t projection_id,
                     const std::vector<size_t>& number_of_voxels,
                     const std::vector<double>& image_sizes,
                     const std::vector<double>& origin,
                     DistanceUnit_t unit) {
  if ( dimension != 0 && 
       number_of_voxels.size() == dimension &&
       image_sizes.size() == dimension
     ){

    // Copy the data in:
    for (size_t i = 0; i < dimension; i ++){
      _image_sizes[i]      = image_sizes[i];
      _number_of_voxels[i] = number_of_voxels[i];
    }

    // Set the origin to 0 if not set
    if (origin.size() == dimension){
      for (size_t i = 0; i < dimension; i ++){
        _origin[i] = origin[i];
      }
    }


    _projection_id    = projection_id;
    _unit             = unit;
    _valid            = true;

  }
  else{
    LARCV_CRITICAL() << "Invalid parameters passed to imagemeta, can not create valid meta." << std::endl;
    throw larbys();
  }

}

template<size_t dimension>
double ImageMeta<dimension>::image_size(size_t axis)       const{
  if (_valid && axis >= 0 && axis < dimension){
    return _image_sizes[axis];
  } 
  else{
    LARCV_CRITICAL() << "Can't return image size of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
double ImageMeta<dimension>::origin(size_t axis)       const{
  if (_valid && axis >= 0 && axis < dimension){
    return _origin[axis];
  } 
  else{
    LARCV_CRITICAL() << "Can't return origin of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
size_t ImageMeta<dimension>::number_of_voxels(size_t axis) const{
  if (_valid && axis >= 0 && axis < dimension){
    return _number_of_voxels[axis];
  } 
  else{
    LARCV_CRITICAL() << "Can't return number_of_voxels of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
size_t ImageMeta<dimension>::total_voxels() const{
  if (_valid){
    size_t i = 1;
    for (size_t axis = 0; axis < dimension; ++ axis) i *= _number_of_voxels[axis];
    return i;
  }
  else{
    LARCV_CRITICAL() << "Can't return total voxels of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
double ImageMeta<dimension>::total_volume() const{
  if (_valid){
    double v = 1.0;
    for (size_t axis = 0; axis < dimension; ++ axis) v *= _number_of_voxels[axis];
    return v;
  }
  else{
    LARCV_CRITICAL() << "Can't return total voxels of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
std::vector<double> ImageMeta<dimension>::voxel_dimensions()  const{
  if (_valid){
    std::vector<double> dims; dims.resize(dimension);
    for (size_t axis = 0; axis < dimension; ++ axis) {
      dims.at(axis) = _image_sizes[axis] / _number_of_voxels[axis];
    }
    return dims;
  }
  else{
    LARCV_CRITICAL() << "Can't return voxel dimensions of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
double ImageMeta<dimension>::voxel_dimensions(size_t axis)    const{
  if (_valid && axis >= 0 && axis < dimension){
    return _image_sizes[axis] / _number_of_voxels[axis];
  }
  else{
    LARCV_CRITICAL() << "Can't return voxel dimensions of invalid meta." << std::endl;
    throw larbys();
  }
}


/// Provide 1-D array index dimension from general coordinates
template<size_t dimension>
size_t ImageMeta<dimension>::index(const std::vector<size_t> & coordinate) const{
  // By convention, the conversion from index to coordinate uses the last coordinate as the most frequently
  // changing, and first coordinate as the most commonly changing.

  if (_valid ){

    if (coordinate.size() != dimension){
      LARCV_CRITICAL() << "Incomplete coordinate submitted for conversion to index, abort." << std::endl;
      throw larbys();
    }

    size_t index = 0;
    size_t stride = 1;
    for (size_t j = 0; j < dimension; j ++ ){
      size_t axis = dimension - j - 1;
      index += coordinate[axis]*stride;
      stride *= _number_of_voxels[axis];
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
template<size_t dimension>
void ImageMeta<dimension>::index( const std::vector<size_t> & coordinates, std::vector<size_t> & output_index) const{


  if (_valid){

    if (coordinates.size() != dimension){
      LARCV_CRITICAL() << "Incomplete coordinates submitted for conversion to index, abort." << std::endl;
      throw larbys();
    }

    output_index.clear();
    output_index.resize(coordinates.size());

    #pragma omp parallel for
    for (size_t i = 0; i < coordinates.size(); i ++){
      // size_t index = 0;
      size_t stride = 1;
    for (size_t j = 0; j < dimension; j ++ ){
        size_t axis = dimension - j - 1;
        output_index.at(i) += coordinates[axis]*stride;
        stride *= _number_of_voxels[axis];
        // dims[axis] = _image_sizes[axis] / _number_of_voxels[axis];
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
template<size_t dimension>
std::vector<size_t> ImageMeta<dimension>::coordinates( size_t index) const{
  if (_valid ){

    std::vector<size_t> coordinates;
    coordinates.resize(dimension);

    // size_t index = 0;
    for (size_t i = 0; i < dimension; i ++ ){
      size_t axis = dimension - i - 1;
      size_t stride = _number_of_voxels[axis];
      coordinates[axis] = index % stride;
      index = index / stride;
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
template<size_t dimension>
void ImageMeta<dimension>::coordinates( const std::vector<size_t> & index,  std::vector<size_t> & output_coordinates) const{

  if (_valid ){

    output_coordinates.resize(index.size() * dimension);

    size_t i_index(0), axis, stride, index_copy, i;
    #pragma omp parallel for private(axis, stride, index_copy)
    for (i_index = 0; i_index < index.size(); ++ i_index){
      index_copy = index.at(i_index);

      for (i = 0; i < dimension; i ++ ){
        axis = dimension - i - 1;
        stride = _number_of_voxels[axis];
        output_coordinates.at(i_index*dimension + axis) = index_copy % stride;
        index_copy = index_copy / stride;
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
template<size_t dimension>
size_t ImageMeta<dimension>::coordinate(size_t index, size_t axis) const{
  if (_valid && axis >= 0 && axis < dimension){

    // size_t index = 0;
    for (size_t loop_axis = dimension - 1; loop_axis >= 0; -- loop_axis) {
      size_t stride = _number_of_voxels[loop_axis];
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
template<size_t dimension>
std::vector<double> ImageMeta<dimension>::position(size_t index) const{
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
template<size_t dimension>
std::vector<double> ImageMeta<dimension>::position(const std::vector<size_t> & coordinates) const{
  if (_valid ){


    std::vector<double> positions;
    positions.resize(dimension);

    // size_t index = 0;
    for (size_t axis = 0; axis < dimension; ++ axis) {
      // Calculate the voxel size:
      double voxel_size = (_image_sizes[axis] / _number_of_voxels[axis] );
      positions.at(axis) = coordinates.at(axis) * voxel_size;
      /// Measure to the center of each voxel
      positions.at(axis) += 0.5*voxel_size + _origin[axis]; 
    }
    
    return positions;
  }
  else{
    LARCV_CRITICAL() << "Can't return positions of invalid meta." << std::endl;
    throw larbys();
  }
}

/// Same as above, but restricted to a single axis
template<size_t dimension>
double ImageMeta<dimension>::position(size_t index, size_t axis) const{
  std::vector<size_t> coords = coordinates(index);    
  return position(coords).at(axis);
}

/// Same as above, but restricted to a single axis
template<size_t dimension>
double ImageMeta<dimension>::position(const std::vector<size_t> & coordinates, size_t axis) const{
  return position(coordinates).at(axis);
}


/// Provide the minimum and maximum real space values of the image.
template<size_t dimension>
std::vector<double> ImageMeta<dimension>::min() const{
  if (_valid){
    std::vector<double> min(_origin, _origin + dimension);
    return min;
  }
  else{
    LARCV_CRITICAL() << "Can't return min of invalid meta." << std::endl;
    throw larbys();
  }
}
template<size_t dimension>
std::vector<double> ImageMeta<dimension>::max() const{
  if (_valid){
    std::vector<double> max;
    max.resize(dimension);
    for (size_t axis = 0; axis < dimension; ++ axis){
      max.at(axis) = _origin[axis] + _image_sizes[axis];
    } 
    return max;
  }
  else{
    LARCV_CRITICAL() << "Can't return min of invalid meta." << std::endl;
    throw larbys();
  }
}

template<size_t dimension>
double ImageMeta<dimension>::min(size_t axis) const{
  return min().at(axis);
}

template<size_t dimension>
double ImageMeta<dimension>::max(size_t axis) const{
  return max().at(axis);
}


//Provide a conversion from a real position to an index or coordinate
template<size_t dimension>
size_t ImageMeta<dimension>::position_to_index(const std::vector<double> & position ) const{
  std::vector<size_t> coords = position_to_coordinate(position);
  return index(coords);
}

template<size_t dimension>
std::vector<size_t> ImageMeta<dimension>::position_to_coordinate(const std::vector<double> & position) const{
  if (_valid && position.size() == dimension){



    std::vector<size_t> coordinates;
    coordinates.resize(dimension);

    std::vector<double> dims = voxel_dimensions();
    
    for (size_t axis = 0; axis < dimension; axis ++){
      double relative_position = position.at(axis) - _origin[axis];
      if (relative_position < 0.0 || relative_position >= _image_sizes[axis]){
        coordinates.at(axis) = kINVALID_INDEX;
      }
      else
        coordinates.at(axis) = (size_t) (relative_position / dims.at(axis));
    }
    return coordinates;
  }
  else{
    LARCV_CRITICAL() << "Can't return position from coordinate of invalid meta." << std::endl;
    throw larbys();
  }
}


/// Map just one position to just one coordinate
template<size_t dimension>
size_t ImageMeta<dimension>::position_to_coordinate(double position, size_t axis) const{
  if (_valid && axis >= 0 && axis < dimension){


    double relative_position = position - _origin[axis];
    if (relative_position < 0.0 || relative_position >= _image_sizes[axis]){
      return kINVALID_INDEX;
    }
    else{ 
      double voxel_dim = voxel_dimensions(axis);
      return  (size_t) (relative_position / voxel_dim);
    }
    
  }
  else{
    LARCV_CRITICAL() << "Can't return position from coordinate of invalid meta." << std::endl;
    throw larbys();
  }
}


template<size_t dimension>
void ImageMeta<dimension>::set_dimension(size_t axis, double image_size, size_t number_of_voxels, double origin){
  if (axis < dimension){
    _image_sizes[axis] = image_size;
    _number_of_voxels[axis] = number_of_voxels;
    if (origin != 0){
      _origin[axis] = origin;      
    } 
  }
  // Update all of the values:

  // Check validity here, which only insists on the total number of voxels
  // to be non zero:

  size_t n_voxels = 1;
  // #pragma omp unroll
  for (size_t i = 0; i < dimension; i++){
    n_voxels *= _number_of_voxels[i];
  }

  if(n_voxels > 0) _valid = true;
  else _valid = false;

}

template<size_t dimension>
bool ImageMeta<dimension>::is_valid() const {

    return _valid;

}



template<size_t dimension>
std::string ImageMeta<dimension>::dump() const
{
  std::stringstream ss;
  ss << "ProjectionID " << id() <<"\n";
  ss << "  N Voxels: (";
  for (size_t i = 0; i < dimension; i ++){
    ss << _number_of_voxels[i];
    if (i != dimension - 1)
      ss << ", ";
  }
  ss << ")\n";
  ss << "  Image Size: (";
  for (size_t i = 0; i < dimension; i ++){
    ss << _image_sizes[i];
    if (i != dimension - 1)
      ss << ", ";
  }
  ss << ")\n";
  ss << "  Origin: (";
  for (size_t i = 0; i < dimension; i ++){
    ss << _origin[i];
    if (i != dimension - 1)
      ss << ", ";
  }
  ss << ")\n";
  // "rows,cols) = (" << _row_count << "," <<
  // _col_count
  //    << ") ... Distance Unit: " << (int)(this-> unit())
  //    << " ... Left Bottom => Right Top " << ((BBox2D*)(this))->dump();
  return ss.str();
}




template class ImageMeta<1>;
template class ImageMeta<2>;
template class ImageMeta<3>;
template class ImageMeta<4>;





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


*/

}  // namespace larcv3

#endif
