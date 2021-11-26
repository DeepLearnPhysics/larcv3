#include "processor.hh"



void init_processor(pybind11::module m){
    init_processbase(m);
    init_processdriver(m);
}

