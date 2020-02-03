#include "base.h"


void init_base(pybind11::module m){
    init_batchdata(m);
    init_queueprocessor(m);
}
