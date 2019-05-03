/**
 * \file BBox.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv3::BBox2D and larcv3::BBox3D
 *
 * @author kazuhiro, cadams
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_BBOX_H__
#define __LARCV3DATAFORMAT_BBOX_H__

#include <iostream>
#include "larcv3/core/dataformat/Point.h"
#include "larcv3/core/dataformat/DataFormatTypes.h"
namespace larcv3 {

  /**
     \class BBox
     \brief Bounding box in ND
  */

  template<size_t dimension>
  class BBox{
  public:

    BBox() {}

    BBox(Point<dimension> min, Point<dimension> max, ProjectionID_t id = kINVALID_PROJECTIONID);    
    
    template<size_t other_dim>
    inline bool operator== (const BBox<other_dim>& rhs) const {
      assert(dimension == other_dim);
      return (_p1 == rhs._p1 && _p2 == rhs._p2); 
    }

    void update(const std::vector<double> & min, const std::vector<double> & max,
                ProjectionID_t id = kINVALID_PROJECTIONID);

    void update(const Point<dimension>& pt1, const Point<dimension>& pt2,
                ProjectionID_t id = kINVALID_PROJECTIONID);

    void update(ProjectionID_t id);

    inline bool empty() const { return (_p1 == _p2); }
    inline const Point<dimension>& origin      () const { return _p1; }
    inline const Point<dimension>& bottom_left () const { return _p1; }
    inline const Point<dimension>& top_right   () const { return _p2; }
    inline Point<dimension> center     () const { return (_p1 + _p2) * 0.5; }
    inline Point<dimension> min        () const { return _p1;}
    inline Point<dimension> max        () const { return _p1;}
    inline Point<dimension> dimensions () const { return _p2 - _p1; }
    inline double           area       () const { return volume(); }
    inline double           volume     () const { 
      double v = 1.0;
      Point<dimension> res = _p2 - _p1;
      for (size_t i = 0; i < dimension; i ++){
        v *= res.x[i];
      }
      return v; 
    }
    
    inline ProjectionID_t id() const { return _id; }
    std::string dump() const;

    BBox overlap(const BBox& box) const;
    BBox inclusive(const BBox& box) const;
    bool contains(const Point<dimension>& point) const;

  private:
    ProjectionID_t _id; ///< ProjectionID_t identifies types of 2D projections to which BBox belongs
    Point<dimension> _p1; ///< bottom-left point coordinate where x1<x2 and y1<y2, ...
    Point<dimension> _p2; ///< top-right point coordinate where x1<x2 and y1<y2, ...
  };

  typedef BBox<2> BBox2D;
  typedef BBox<3> BBox3D;

}
#endif
/** @} */ // end of doxygen group
