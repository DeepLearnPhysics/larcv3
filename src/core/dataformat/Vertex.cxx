#ifndef __LARCV3DATAFORMAT_VERTEX_CXX__
#define __LARCV3DATAFORMAT_VERTEX_CXX__

#include <sstream>
#include "larcv3/core/dataformat/Vertex.h"
#include "larcv3/core/base/larbys.h"
#include "larcv3/core/base/larcv_logger.h"

namespace larcv3 {

  // Interaction ID default constructor
  // Vertex::Vertex()
  //   : _x(0)
  //   , _y(0)
  //   , _z(0)
  //   , _t(0)
  // {approx();}

  Vertex::Vertex(double x, double y, double z, double t)
    : _x(x), _y(y), _z(z), _t(t)
  // {approx();}
  {}

  void Vertex::reset(){
    _x = _y = _z = _t = 0;
  }

  void Vertex::reset(double x, double y, double z, double t)
  {
    _x = x;
    _y = y;
    _z = z;
    _t = t;
    // approx();
  }

  const larcv3::Point2D Vertex::as_point2d(larcv3::PointType_t point_type) const
  {

    std::array<double, 2> data = {};

    switch(point_type) {
      case kPointXY:
        data[0] = _x; data[1] = _y;
      case kPointYZ:
        data[0] = _y; data[1] = _z;
      case kPointZX:
        data[0] = _z; data[1] = _x;
      case kPoint3D:
        LARCV_CRITICAL() << "Wrong point type (Point3D) in Vertex::as_point2d" << std::endl;
        throw larbys();
    }
    return Point2D(data);
  }

  const larcv3::Point3D Vertex::as_point3d() const
  {
    Point3D ret;
    ret.x[0] = _x; ret.x[1] = _y; ret.x[2] = _z;
    return ret;
  }

  void Vertex::as_point(larcv3::PointType_t point_type, double * x, double * y, double * z)
  {
    switch(point_type) {
      case kPointXY:
        *x = _x;
        *y = _y;
        break;
      case kPointYZ:
        *x = _y;
        *y = _z;
        break;
      case kPointZX:
        *x = _z;
        *y = _x;
        break;
      case kPoint3D:
        *x = _x;
        *y = _y;
        *z = _z;
        break;
    }
  }

  std::string Vertex::dump() const
  {
    std::stringstream ss;
    ss << "x = " << _x << " ; y = " << _y << " ; z = " << _z << " ; t = " << _t << std::endl;
    return ss.str();
  }

  // void Vertex::approx()
  // {
  //   _x = (double)( ((double)((signed long long)(_x * 1.e6)) * 1.e-6 ));
  //   _y = (double)( ((double)((signed long long)(_y * 1.e6)) * 1.e-6 ));
  //   _z = (double)( ((double)((signed long long)(_z * 1.e6)) * 1.e-6 ));
  //   _t = (double)( ((double)((signed long long)(_t * 1.e6)) * 1.e-6 ));
  // }
}


#include <pybind11/operators.h>

void init_vertex(pybind11::module m){

    // using larcv3::Vertex = larcv3::Vertex;
    pybind11::class_<larcv3::Vertex> vertex(m, "Vertex");
    vertex.def(pybind11::init<>());
    vertex.def(pybind11::init<double, double, double, double>());
    vertex.def("reset", (void (larcv3::Vertex::*)())(&larcv3::Vertex::reset), "Reset");
    vertex.def("reset", (void (larcv3::Vertex::*)(double, double, double, double))(&larcv3::Vertex::reset), "Reset");
    vertex.def("x", &larcv3::Vertex::x);
    vertex.def("y", &larcv3::Vertex::y);
    vertex.def("z", &larcv3::Vertex::z);
    vertex.def("t", &larcv3::Vertex::t);
    vertex.def("as_point2d", &larcv3::Vertex::as_point2d);
    vertex.def("as_point3d", &larcv3::Vertex::as_point3d);
    vertex.def(pybind11::self == pybind11::self);
    vertex.def(pybind11::self != pybind11::self);
    vertex.def(pybind11::self < pybind11::self);
    vertex.def("dump", &larcv3::Vertex::dump);
    vertex.def("__repr__",&larcv3::Vertex::dump);


/*


    std::string dump() const;


*/
}

#endif
