/**
 * \file BBox.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class larcv3::BBox2D and larcv3::BBox3D
 *
 * @author kazuhiro, cadams
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_BBOX_H__
#define __LARCV3DATAFORMAT_BBOX_H__

#include <iostream>
#include "larcv3/core/dataformat/DataFormatTypes.h"
#include "larcv3/core/dataformat/ImageMeta.h"



namespace larcv3 {

  /**
     \class BBox
     \brief Bounding box in ND
  */

  template<size_t dimension>
  class BBox{
  public:

    BBox();

    /**
     * @brief Non default constructor
     */
    BBox(const std::array<double, dimension>& centroid, 
         const std::array<double, dimension>& half_length,
         const std::array<double, dimension*dimension> & rotation = {});

    
    /**
     * @brief      Return rotation, flattened
     *
     * @return     array of length dimension**2
     */
    std::array<double, dimension*dimension > identity_rotation();

    /**
     * @brief      Return Centroid
     *
     */
    const std::array<double, dimension>& centroid() const {return _centroid;} 
    /**
     * @brief      Return half length
     *
     */
    const std::array<double, dimension>& half_length() const {return _half_length;}

    /**
     * @brief      Return rotation_matrix
     *
     * @return     array of size dimension**2
     */
    const std::array<double, dimension*dimension> & rotation_matrix() const {return _rotation;}

    /**
     * @brief      Equality operator.
     *
     * @param[in]  rhs   The right hand side
     *
     * @return     The result of the equality
     */
    inline bool operator== (const BBox<dimension>& rhs) const {
      return ( _centroid    == rhs._centroid && 
               _half_length == rhs._half_length && 
               _rotation    == rhs._rotation);
    }

    /**
     * @brief      Convert contents to readable string
     *
     * @return     Formatted string
     */
    std::string dump() const;



  private:

    // central location of the bbox.  Rotations, if used, are around this point.
    std::array<double, dimension> _centroid;             
    
    // half length of BBox in each dimension.
    std::array<double, dimension> _half_length;          

    // unitary rotation matrix, defaults to identity.
    std::array<double, dimension*dimension> _rotation;  

  public:
    /**
     * @brief      Gets the datatype.
     *
     * @return     The datatype.
     */
    static hid_t get_datatype() {
      hid_t datatype;
      datatype = H5Tcreate (H5T_COMPOUND, sizeof (BBox));

      hsize_t array_dimensions[1];
      array_dimensions[0] = dimension;

      hid_t double_type   = H5Tarray_create(larcv3::get_datatype<double>(), 1, array_dimensions);

      hsize_t rotation_dimensions[1];
      rotation_dimensions[0] = dimension*dimension;
      hid_t rotation_type = H5Tarray_create(larcv3::get_datatype<double>(), 1, rotation_dimensions);

      H5Tinsert (datatype, "centroid",
                 HOFFSET (BBox, _centroid),
                 double_type);
      H5Tinsert (datatype, "half_length",
                 HOFFSET (BBox, _half_length),
                 double_type);
      H5Tinsert (datatype, "rotation",
                 HOFFSET (BBox, _rotation),
                 rotation_type);

      return datatype;
    }

  };

  typedef BBox<2> BBox2D;
  typedef BBox<3> BBox3D;


  template<size_t dimension>
  class BBoxCollection{

    // template <size_t dimension> friend class EventBBox<dimension>;
    template<size_t> friend class EventBBox;

  public:

    BBoxCollection(){};
    BBoxCollection(ImageMeta<dimension> meta):_meta(meta){};

    /**
     * @brief      Get number of BBox
     *
     * @return     Number of BBoxes
     */
    inline size_t size() const { return _bbox_v.size(); }
    /**
     * @brief      Access specific BBox
     *
     * @param[in]  id    The identifier
     *
     * @return     larcv3::BBox
     */
    const larcv3::BBox<dimension> & bbox(InstanceID_t id) const {return _bbox_v.at(id);}
    /**
     * @brief      Access all BBox as immutable vector
     *
     * @return     Const ref to vector of bboxes
     */
    inline const std::vector<larcv3::BBox<dimension> >& as_vector() const
    { return _bbox_v; }

    //
    // Write-access
    //

    /**
     * @brief      Clear everything
     */
    inline void clear_data() { _bbox_v.clear(); }
    /**
     * @brief      Resize bbox array
     *
     * @param[in]  num   The number of boxes
     */
    inline void resize(const size_t num)
    { this->clear_data(); _bbox_v.resize(num); }

    /**
     * @brief      Access non-const reference of a specific BBox
     *
     * @param[in]  id    The identifier
     *
     * @return     Mutable ref top bbox
     */
    larcv3::BBox<dimension>& writeable_bbox(const InstanceID_t id) {return _bbox_v.at(id);}

    /**
     * @brief      Move a BBox into a collection.
     *
     * @param      bbox  The bounding box
     */
    void emplace(larcv3::BBox<dimension> && bbox){_bbox_v.push_back(bbox);}

    /**
     * @brief      Set a BBox into a collection.
     *
     * @param[in]  bbox  The bounding box
     */
    void append(const larcv3::BBox<dimension> & bbox) {_bbox_v.push_back(bbox);}
    
    /**
     * @brief      Move an entire collection of bboxes
     *
     * @param      orig  The original
     */
    void move(larcv3::BBoxCollection<dimension> && orig)
    { _bbox_v = std::move(orig._bbox_v); }

    /**
     * @brief      Return a new BBoxCollection that is this one, but compressed/downsampled
     *
     * @param      compression  The compression level (one value per dimension)
     * @param      pool_type  The pool type
     */
    larcv3::BBoxCollection<dimension> compress(size_t compression, PoolType_t pool_type) const;

    /**
     * @brief      Return a new BBoxCollection that is this one, but compressed/downsampled
     *
     * @param      compression  The compression level (one value per dimension)
     * @param      pool_type  The pool type
     */
    larcv3::BBoxCollection<dimension> compress(std::array<size_t, dimension> compression, PoolType_t pool_type) const;


    /// Meta setter
    void meta(const larcv3::ImageMeta<dimension>& meta){_meta = meta;}

    /// Meta getter
    const ImageMeta<dimension>& meta() const{return _meta;}

  private:
    std::vector<BBox<dimension> > _bbox_v;
    ImageMeta<dimension>          _meta;
  
  };

  typedef BBoxCollection<2> BBoxCollection2D;
  typedef BBoxCollection<3> BBoxCollection3D;

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>

void init_bbox(pybind11::module m);
#endif


#endif
/** @} */ // end of doxygen group
