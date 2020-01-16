#include "larbys.h"

void init_larbys(pybind11::module m){
      pybind11::class_<larcv3::larbys>(m, "larbys")
        .def(pybind11::init<const std::string &>(), pybind11::arg("msg")="")
        .def("what", &larcv3::larbys::what)
        ;
}
