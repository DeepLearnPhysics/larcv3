#ifndef __LARCV3DATAFORMAT_POINT_CXX
#define __LARCV3DATAFORMAT_POINT_CXX

#include <pybind11/stl.h>
#include <pybind11/operators.h>


#include "larcv3/core/dataformat/Point.h"


namespace larcv3{
template class Point<2>;
template class Point<3>;
}

template<size_t dimension>
void init_point_base(pybind11::module m){
    std::string classname = "Point" + std::to_string(dimension) + "D";
    pybind11::class_<larcv3::Point<dimension>> point(m, classname.c_str());
    point.def(pybind11::init<>());
    point.def(pybind11::init<std::array<double, dimension> > ());
    point.def(pybind11::init<larcv3::Point<dimension> > ());
    point.def(pybind11::self == pybind11::self);
    point.def(pybind11::self != pybind11::self);
    point.def(pybind11::self *= double());
    point.def(pybind11::self /= double());
    point.def(pybind11::self -= pybind11::self);
    point.def(pybind11::self += pybind11::self);
    point.def(pybind11::self * double());
    point.def(pybind11::self / double());
    point.def(pybind11::self + pybind11::self);
    point.def(pybind11::self - pybind11::self);
    point.def("squared_distance", &larcv3::Point<dimension>::squared_distance);
    point.def("distance",         &larcv3::Point<dimension>::distance);
    point.def("direction",        &larcv3::Point<dimension>::direction);
    point.def_readwrite("x", &larcv3::Point<dimension>::x);
}

void init_point(pybind11::module m){
    // init_point2D(m);
    init_point_base<2>(m);
    init_point_base<3>(m);
}

#endif
