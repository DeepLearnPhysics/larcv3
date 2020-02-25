#include "larcv_base.h"

void init_larcv_base(pybind11::module m){
      pybind11::class_<larcv3::larcv_base>(m, "larcv_base")
        .def(pybind11::init<const std::string &>(), pybind11::arg("name")="larcv_base")
        .def("logger", &larcv3::larcv_base::logger)
        .def("set_verbosity", &larcv3::larcv_base::set_verbosity)
        .def("name", &larcv3::larcv_base::name)
        ;
}
