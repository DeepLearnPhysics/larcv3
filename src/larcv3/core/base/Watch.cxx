#include "Watch.h"

void init_Watch(pybind11::module m){
      pybind11::class_<larcv3::Watch> watch(m, "Watch");

      watch.doc() = R"pbdoc(
        Watch
        =====

        Basic C++ timer object, used to track algorithm performance.
      )pbdoc";

      watch.def(pybind11::init<>(),
      R"pbdoc(
        Construct a C++ timer object for measuring algorithm performance in C++ code.
      )pbdoc");
      watch.def("Start", &larcv3::Watch::Start,
      R"pbdoc(
        Start the timer.
      )pbdoc");
      watch.def("WallTime", &larcv3::Watch::WallTime,
      R"pbdoc(
        Get the current walltime.
      )pbdoc");
      watch.def("CPUTime", &larcv3::Watch::CPUTime,
      R"pbdoc(
        Report CPU Time since Start was called.
      )pbdoc");

        ///TODO Finish the implementation of the functions here
}