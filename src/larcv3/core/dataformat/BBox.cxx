#ifndef __LARCV3DATAFORMAT_BBOX_CXX__
#define __LARCV3DATAFORMAT_BBOX_CXX__

#include "larcv3/core/dataformat/BBox.h"
#include <cmath>
#include <set>
#include <sstream>
#include "larcv3/core/base/larbys.h"

namespace larcv3 {


  template <size_t dimension>
  BBox<dimension>::BBox(){
    for (size_t i = 0; i < dimension; i++){
      _centroid[i] = 0.0;
      _half_length[i] = 1.0;
    }

    _rotation = this->identity_rotation();
  }

  template <size_t dimension>
  BBox<dimension>::BBox(
        const std::array<double, dimension>& centroid, 
        const std::array<double, dimension>& half_length,
        const std::array<double, dimension*dimension> & rotation)
    : _centroid(centroid),
      _half_length(half_length)
    // rotation(_rotation)
  {
    bool rotation_set = false;
    for (size_t i = 0; i < dimension*dimension; i ++){
      if (rotation[i] != 0){
        rotation_set = true;
        break;
      }
    }

    if (!rotation_set) _rotation = this->identity_rotation();

  }

  template <size_t dimension>
  std::array<double, dimension*dimension > BBox<dimension>::identity_rotation(){
    std::array<double, dimension*dimension > rot_mat = {0.};

    for (size_t i = 0; i < dimension*dimension; i += dimension) rot_mat[i] = 1.0;

    return rot_mat;
  }


  template<size_t dimension>
  std::string BBox<dimension>::dump() const
  {
    std::stringstream ss;
    // Add the centroid:
    ss << "Centroid: (";
    for (size_t i = 0; i < dimension; i ++){
      ss << _centroid[i];
      if (i != dimension -1) ss << ",";
    }

    ss << "), half lengths: (";
    for (size_t i = 0; i < dimension; i ++){
      ss << _half_length[i];
      if (i != dimension -1) ss << ",";
    }
    ss << ")" << std::endl;
    return ss.str();
  }

  template<size_t dimension>
  larcv3::BBoxCollection<dimension> BBoxCollection<dimension>::compress(size_t compression, PoolType_t pool_type) const
  {

    std::array<size_t, dimension> comp;
    for (size_t i = 0; i < dimension; ++i ) comp[i] = compression;

    return this->compress(comp, pool_type);
  }

  template<size_t dimension>
  larcv3::BBoxCollection<dimension> BBoxCollection<dimension>::compress(std::array<size_t, dimension> compression, PoolType_t pool_type) const
  {
    ImageMeta<dimension> compressed_meta = this->meta().compress(compression);

    BBoxCollection<dimension> output(compressed_meta);

    // Centroid and legth are in absolute coordinates, so not affected by downsampling
    for (size_t i = 0; i < this->size(); i++) {
      BBox<dimension> bbox(this->bbox(i).centroid(),
                           this->bbox(i).half_length(),
                           this->bbox(i).rotation_matrix());
      output.append(bbox);
    }

    return output;
  }

template class BBox<2>;
template class BBox<3>;

template class BBoxCollection<2>;
template class BBoxCollection<3>;

template<> std::string as_string<BBox<2>>() {return "BBox2D";}
template<> std::string as_string<BBox<3>>() {return "BBox3D";}

template<> std::string as_string<BBoxCollection<2>>() {return "BBoxCollection2D";}
template<> std::string as_string<BBoxCollection<3>>() {return "BBoxCollection3D";}

}


#include <pybind11/operators.h>
#include <pybind11/stl.h>


template<size_t dimension>
void init_bbox_instance(pybind11::module m){
    using Class = larcv3::BBox<dimension>;
    pybind11::class_<Class> bbox(m, larcv3::as_string<Class>().c_str());

    bbox.doc() = R"pbdoc(
      Represents a bounding box in N dimensions, defined by a centroid and half_length in each axis from the centroid.
      The rotation component is currently not used but provided in the data store implementation for future use.
      Used to represent, for example, the location of a particle interaction, a bounding box for an instance of an object,
      etc.
    )pbdoc";

    bbox.def(pybind11::init<>(), "Default Constructor");
    bbox.def(pybind11::init<const std::array<double, dimension>&, 
                            const std::array<double, dimension>&,
                            const std::array<double, dimension*dimension>
                           > (),
             pybind11::arg("centroid"),
             pybind11::arg("half_length"),
             pybind11::arg("rotation"),
             "Constructor using centroid, half_length, and rotation."
    );
    bbox.def(pybind11::init<const std::array<double, dimension>&, 
                            const std::array<double, dimension>&
                           > (),
             pybind11::arg("centroid"),
             pybind11::arg("half_length"),
             "Constructor using centroid and half_length."
    );

    bbox.def("centroid",           &Class::centroid, "Get the value of the centroid.");
    bbox.def("half_length",        &Class::half_length, "Get the value of the half_length.");
    bbox.def("rotation_matrix",    &Class::rotation_matrix, "Get the value of the rotation matrix.");
    bbox.def("identity_rotation",  &Class::identity_rotation, "Get the value of the identity rotation.");

    bbox.def("dump",               &Class::dump, "Return a string representation of the BBox");
    bbox.def("__repr__",           &Class::dump, "Return a string representation of the BBox");

}

template<size_t dimension>
void init_bbox_collection(pybind11::module m){
    using Class = larcv3::BBoxCollection<dimension>;
    pybind11::class_<Class> bbox_c(m, larcv3::as_string<Class>().c_str());
    bbox_c.def(pybind11::init<>());

    bbox_c.doc() = R"pbdoc(
      A collection of Bounding Boxes in N dimensions.  Generally used to represent a sequence of Bounding
      Boxes all within the same image.
    )pbdoc";

    bbox_c.def("bbox",           &Class::bbox);
    bbox_c.def("as_vector",      &Class::as_vector);
    bbox_c.def("clear_data",     &Class::clear_data);
    bbox_c.def("resize",         &Class::resize);
    bbox_c.def("writeable_bbox", &Class::writeable_bbox);
    bbox_c.def("append",         &Class::append);
    bbox_c.def("size",           &Class::size);
    bbox_c.def("meta", (const larcv3::ImageMeta<dimension>& (Class::*)() const )(&Class::meta), 
      pybind11::return_value_policy::reference);
    bbox_c.def("meta", (void (Class::*)(const larcv3::ImageMeta<dimension>& )  )(&Class::meta), 
      pybind11::arg("meta"), 
      pybind11::return_value_policy::reference);

}

void init_bbox(pybind11::module m){
  // Here, this creates a wrapper for the classes we're interested in:
  init_bbox_instance<2>(m);
  init_bbox_instance<3>(m);

  init_bbox_collection<2>(m);
  init_bbox_collection<3>(m);
}


#endif
