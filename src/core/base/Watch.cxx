#include "Watch.h"

void init_Watch(pybind11::module m){
      pybind11::class_<larcv3::Watch>(m, "Watch")
        .def(pybind11::init<>())
        .def("Start", &larcv3::Watch::Start)
        .def("WallTime", &larcv3::Watch::WallTime)
        .def("CPUTime", &larcv3::Watch::CPUTime)
        // .def("default_level", (larcv3::msg::Level_t (larcv3::logger::*)()) &larcv3::logger::default_level)
        // .def("default_level", (void (larcv3::logger::*)(larcv3::msg::Level_t)) &larcv3::logger::default_level)
        ;

        ///TODO Finish the implementation of the functions here
}