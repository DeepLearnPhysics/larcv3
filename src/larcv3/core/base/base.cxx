#include "base.h"


void init_base(pybind11::module m){
    init_larbys(m);
    init_larcv_base(m);
    init_logger(m);
    init_PSet(m);
    init_Watch(m);
}
