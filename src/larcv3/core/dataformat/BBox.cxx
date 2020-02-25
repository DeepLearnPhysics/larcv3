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

template<> std::string as_string<BBox<2>>() {return "BBox2D";}
template<> std::string as_string<BBox<3>>() {return "BBox3D";}

}


#include <pybind11/operators.h>



template<size_t dimension>
void init_bbox_instance(pybind11::module m){
    using Class = larcv3::BBox<dimension>;
    pybind11::class_<Class> bbox(m, larcv3::as_string<Class>().c_str());
    bbox.def(pybind11::init<>());
    bbox.def(pybind11::init<larcv3::Point<dimension>, larcv3::Point<dimension>, larcv3::ProjectionID_t > ());
    bbox.def(pybind11::self == pybind11::self);

    bbox.def("update",
      (void (Class::*)
        (const std::vector<double> & , const std::vector<double> & , larcv3::ProjectionID_t )  )
      (&Class::update));
    bbox.def("update", 
      (void (Class::*)
        (const larcv3::Point<dimension> & , const larcv3::Point<dimension> & , larcv3::ProjectionID_t )  )
      (&Class::update));
    bbox.def("update", 
      (void (Class::*)
        (larcv3::ProjectionID_t) )
      (&Class::update));


    bbox.def("empty",            &Class::empty);
    bbox.def("origin",           &Class::origin);
    bbox.def("bottom_left",      &Class::bottom_left);
    bbox.def("top_right",        &Class::top_right);
    bbox.def("center",           &Class::center);
    bbox.def("min",              &Class::min);
    bbox.def("max",              &Class::max);
    bbox.def("dimensions",       &Class::dimensions);
    bbox.def("area",             &Class::area);
    bbox.def("volume",           &Class::volume);
    bbox.def("id",               &Class::id);
    bbox.def("dump",             &Class::dump);
    bbox.def("__repr__",         &Class::dump);
    bbox.def("overlap",          &Class::overlap);
    bbox.def("inclusive",        &Class::inclusive);
    bbox.def("contains",         &Class::contains);


}

void init_bbox(pybind11::module m){
  // Here, this creates a wrapper for the classes we're interested in:
  init_bbox_instance<2>(m);
  init_bbox_instance<3>(m);
}


#endif
