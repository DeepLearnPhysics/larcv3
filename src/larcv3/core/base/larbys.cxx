#include "larbys.h"

void init_larbys(pybind11::module m){
      pybind11::class_<larcv3::larbys> larbys(m, "larbys");

      larbys.doc() = R"pbdoc(

        Base expection for larcv.
      )pbdoc";

      larbys.def(pybind11::init<const std::string &>(), pybind11::arg("msg")="", 
      R"pbdoc(
        Construction an exception - not expected to be used from Python.
      )pbdoc");
      larbys.def("what", &larcv3::larbys::what,
      R"pbdoc(
        Get the details of this exception.
      )pbdoc");
}
