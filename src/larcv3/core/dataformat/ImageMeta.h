/**
 * \file ImageMeta.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv3::ImageMeta
 *
 * @author kazuhiro
 * @author cadams
 *
 * This class defines an abstract rectangular voxelized region in N dimension space
 * The class is represented by it's number of dimensions, as well as the number
 * of voxels in each dimension and the absolute length of each dimension in real space.
 * A "position" is the real valued coordinate inside this space
 * A "coordinate" is the voxelized location inside this space, ranging from 0 to n_dims
 * An "index" is a unique, single value for each coordinate.
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_IMAGEMETA_H__
#define __LARCV3DATAFORMAT_IMAGEMETA_H__

#include <iostream>
#include "larcv3/core/dataformat/DataFormatTypes.h"
#include "larcv3/core/base/larbys.h"

namespace larcv3 {

template<size_t dimension>
class ImageMeta {
 public:
  /// Default constructor: Does nothing, valid defaults to false
  ImageMeta();

  /// Constructor with arguments: ndims, dims, image_sizes, unit.
  ImageMeta(size_t projection_id,
            const std::vector<size_t>& number_of_voxels,
            const std::vector<double>& image_sizes, 
            const std::vector<double>& origin = std::vector<double>(),
            DistanceUnit_t unit = kUnitUnknown);

  // Comparison operators:
  inline bool operator==(const ImageMeta<dimension> & rhs) const {
    for (size_t i = 0; i < dimension; i ++){
      if (_image_sizes[i]       != rhs._image_sizes[i])
        return false;
      if (_number_of_voxels[i]  != rhs._number_of_voxels[i])
        return false;
      if (_origin[i]              != rhs._origin[i])
        return false;
    }
    return (
      _projection_id == rhs._projection_id &&
      _unit          == rhs._unit);
  }

  inline bool operator!=(const ImageMeta<dimension> & rhs) const {
    return !((*this) == rhs);
  }



  inline const size_t projection_id() const{ return _projection_id;}
  inline const size_t id() const{return _projection_id;}
  /// Operators to access information about the image meta:

  inline const double * image_size()       const {return _image_sizes;}
  inline const size_t * number_of_voxels() const {return _number_of_voxels;}
  inline const double * origin()           const {return _origin;}


  inline size_t n_dims()  const { return dimension; }

  double image_size(size_t axis)       const;
  size_t number_of_voxels(size_t axis) const;
  double origin(size_t axis)           const;

  size_t total_voxels() const;
  double total_volume() const;

  std::vector<double> voxel_dimensions()  const;
  double voxel_dimensions(size_t axis)    const;

  /// 2D length unit
  inline DistanceUnit_t unit() const { return _unit; }

  /// Provide 1-D array index dimension from general coordinates
  size_t index(const std::vector<size_t> & coordinate) const;

  /// Vectorized version for converting all coordinates to index:
  /// Coordinates is flattened here as [x0, y0, z0, x1, y1, z1, x2, y2, z2, x3 ....]
  void index( const std::vector<size_t> & coordinates, std::vector<size_t> & output_index) const;


  /// Convert 1D index to overall coordinates
  std::vector<size_t> coordinates( size_t index) const;
  /// Vectorized version for converting all indexes into coordinates
  /// Coordinates is flattened here as [x0, y0, z0, x1, y1, z1, x2, y2, z2, x3 ....]
  void coordinates( const std::vector<size_t> & index,  std::vector<size_t> & output_coordinates) const;



  /// Convert 1D index to overall coordiante along specified axis
  size_t coordinate(size_t index, size_t axis) const;
  /// There is no vectorized version of the single axis coordinate, open an issue if you need it.


  /// Provide absolute coordinate of the center of a specified pixel index
  std::vector<double> position(size_t index) const;



  /// Provide absolute coordinate of the center of a specified pixel (row,col)
  std::vector<double> position(const std::vector<size_t> & coordinates) const;


  /// Same as above, but restricted to a single axis
  double position(size_t index, size_t axis) const;

  /// Same as above, but restricted to a single axis
  double position(const std::vector<size_t> & coordinates, size_t axis) const;


  /// Provide the minimum and maximum real space values of the image.
  std::vector<double> min() const;
  std::vector<double> max() const;
  double min(size_t axis) const;
  double max(size_t axis) const;


  //Provide a conversion from a real position to an index or coordinate
  size_t position_to_index(const std::vector<double> & position ) const;
  std::vector<size_t> position_to_coordinate(const std::vector<double> & position) const;
  /// Map just one position to just one coordinate
  size_t position_to_coordinate(double position, size_t axis) const;

  // This function is useful to interactively build up a meta object
  // Each dimension ImageMeta is meant to store the number of voxels as well as the physical space those voxels represent
  // image_size is the physical length of the dimension, regardless of voxel size
  // number_of_voxels is the total number of voxels used along the full image_size
  // origin is the lowest value in physical space of this dimension.  Typically this is 0 but doesn't need to be.
  void set_dimension(size_t axis, double image_size, size_t number_of_voxels, double origin = 0);

  inline void set_projection_id(size_t projection_id){_projection_id = projection_id;}

  bool is_valid() const;


  // These functions are mostly for historical compatibility and consistence:
  inline size_t cols() const {return _number_of_voxels[0];}
  inline size_t rows() const {return _number_of_voxels[1];}

  // inline void update(size_t row_count, size_t col_count) {
  //   _row_count = row_count;
  //   _col_count = col_count;
  // }
  // /// Reset origin coordinate
  // inline void reset_origin(double x, double y) {
  //   BBox2D::update(x, y, max_x(), max_y());
  // }

  // /// Check if there's an overlap. If so return overlapping bounding box
  // ImageMeta overlap(const ImageMeta& meta) const;
  // /// Construct a union bounding box
  // ImageMeta inclusive(const ImageMeta& meta) const;

  /// Dump info in text
  std::string dump() const;


#ifndef SWIG
  public: 
    static hid_t get_datatype() {
      hid_t datatype;
      herr_t status;
      datatype = H5Tcreate (H5T_COMPOUND, sizeof (ImageMeta));

      hsize_t array_dimensions[1];
      array_dimensions[0] = dimension;


      hid_t double_type = H5Tarray_create(larcv3::get_datatype<double>(), 1, array_dimensions);
      hid_t size_t_type = H5Tarray_create(larcv3::get_datatype<size_t>(), 1, array_dimensions);

      status = H5Tinsert (datatype, "valid",
                  HOFFSET (ImageMeta, _valid),            larcv3::get_datatype<bool>());
      status = H5Tinsert (datatype, "projection_id",
                  HOFFSET (ImageMeta, _projection_id),    larcv3::get_datatype<size_t>());
      status = H5Tinsert (datatype, "image_sizes",
                  HOFFSET (ImageMeta, _image_sizes),      double_type);
      status = H5Tinsert (datatype, "number_of_voxels",
                  HOFFSET (ImageMeta, _number_of_voxels), size_t_type);
      status = H5Tinsert (datatype, "origin", 
                  HOFFSET (ImageMeta, _origin),           double_type);
      return datatype;
    }
#endif


 protected:
  // ImageIndex_t   _image_id;  ///< Associated image ID (of the same producer
  // name)

  bool _valid;  ///< Boolean set to true only if voxel parameters are properly set
  // size_t _n_dims;
  size_t _projection_id;
  double _image_sizes[dimension];  ///< image size in [_unit] along each dimension
  size_t _number_of_voxels[dimension];  ///< Total number of voxels in each dimension
  double _origin[dimension]; ///The location of index==0


  DistanceUnit_t _unit;  ///< length unit
};

typedef ImageMeta<1> ImageMeta1D;
typedef ImageMeta<2> ImageMeta2D;
typedef ImageMeta<3> ImageMeta3D;
typedef ImageMeta<4> ImageMeta4D;


}  // namespace larcv3

#endif
/** @} */  // end of doxygen group
