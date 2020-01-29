#ifndef __LARCV3DATAFORMAT_BBOX_CXX__
#define __LARCV3DATAFORMAT_BBOX_CXX__

#include "larcv3/core/dataformat/BBox.h"
#include <cmath>
#include <set>
#include <sstream>
#include "larcv3/core/base/larbys.h"

namespace larcv3 {

  template<size_t dimension>
  BBox<dimension>::BBox(Point<dimension> min, Point<dimension> max, ProjectionID_t id )
    : _id(id), _p1(min), _p2(max)
  {
    for (size_t i = 0; i < dimension; i++){
      if(_p1.x[i] > _p2.x[i]) throw larbys("min > max not allowed for BBox construction!");
    }
  }

  template<size_t dimension>
  void BBox<dimension>::update(const std::vector<double> & min, const std::vector<double> & max, ProjectionID_t id)
  {
    if(min.size() != max.size() ||
       min.size() != dimension){
      throw larbys("Must use min and max of correct length for BBox update!");
    }
    for (size_t i = 0; i < dimension; i++){
      if(min[i] > max[i]) throw larbys("min > max not allowed for BBox construction!");
    }
    for (size_t i = 0; i < dimension; i++){
      _p1.x[i] = min[i];
      _p2.x[i] = max[i];
    }
    _id = id;
  }

  template<size_t dimension>
  void BBox<dimension>::update(const Point<dimension>& p1, const Point<dimension>& p2, ProjectionID_t id)
  {    

    for (size_t i = 0; i < dimension; i++){
      if(p1.x[i] > p2.x[i]) throw larbys("min > max not allowed for BBox construction!");
    }
    for (size_t i = 0; i < dimension; i++){
      _p1.x[i] = p1.x[i];
      _p2.x[i] = p2.x[i];
    }
    _id = id;
  }

  template<size_t dimension>
  void BBox<dimension>::update(ProjectionID_t id)
  { _id = id; }

  template<size_t dimension>
  BBox<dimension> BBox<dimension>::overlap(const BBox<dimension>& box) const
  {

    Point<dimension> overlap_p1;
    Point<dimension> overlap_p2;

    for (size_t i = 0; i < dimension; i ++){
      overlap_p1.x[i] = std::max(this->_p1.x[i], box.min().x[i]); // Pick larger min coordinate
      overlap_p2.x[i] = std::min(this->_p1.x[i], box.max().x[i]); // Pick smaller max coordinate
      if (overlap_p2.x[i] < overlap_p1.x[i]){ // verify there is overlap
              std::stringstream ss;
        ss << "No overlap found along dimension " << i << ":\n" 
           << "  this X: " << this->_p1.x[i] << " ... the other X: " << box.min().x[i] << std::endl;
        throw larbys(ss.str());
      }
    }

    return BBox<dimension>(overlap_p1, overlap_p2);
  }

  template<size_t dimension>
  BBox<dimension> BBox<dimension>::inclusive(const BBox<dimension>& box) const
  {


    Point<dimension> overlap_p1;
    Point<dimension> overlap_p2;

    for (size_t i = 0; i < dimension; i ++){

      overlap_p1.x[i] = std::min(this->_p1.x[i], box.min().x[i]); // Pick smaller min coordinate
      overlap_p2.x[i] = std::max(this->_p1.x[i], box.max().x[i]); // Pick larger max coordinate
    }

    return BBox<dimension>(overlap_p1, overlap_p2);

  }

  template<size_t dimension>
  bool BBox<dimension>::contains(const Point<dimension>& point) const
  {
    bool contained(true);
    for (size_t i = 0; i < dimension; i ++){
      contained = ( contained && point.x[i] >= _p1.x[i] && point.x[i] <= _p2.x[i]);
    }
    return contained;
  }

  template<size_t dimension>
  std::string BBox<dimension>::dump() const
  {
    std::stringstream ss;
    // Add the first point:
    ss << "    (";
    for (size_t i = 0; i < dimension; i ++){
      ss << _p1.x[i];
      if (i != dimension -1) ss << ",";
    }

    ss << ") => (";
    // Add the second point:
    for (size_t i = 0; i < dimension; i ++){
      ss << _p2.x[i];
      if (i != dimension -1) ss << ",";
    }

    ss << ")" << std::endl;;
    return ss.str();
  }

template class BBox<2>;
template class BBox<3>;


template<size_t dimension>
void init_bbox_instance(pybind11::module m){
    std::string classname = "BBox" + dimension;
    pybind11::class_<larcv3::BBox<dimension>>(m, classname.c_str())
        .def(pybind11::init<>())
        // .def("logger", &larcv3::BBox<dimension>::logger)
        // .def("set_verbosity", &larcv3::BBox<dimension>::set_verbosity)
        // .def("name", &larcv3::BBox<dimension>::name)
        ;

/*


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

*/


}


void init_bbox(pybind11::module m){
  // Here, this creates a wrapper for the classes we're interested in:
  init_bbox_instance<2>(m);
  init_bbox_instance<3>(m);
}


}

#endif
