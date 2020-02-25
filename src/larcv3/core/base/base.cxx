#include "base.h"


void init_base(pybind11::module m){
    init_larbys(m);
    init_larcv_base(m);
    init_logger(m);
    init_PSet(m);
    init_Watch(m);

    m.attr("kINVALID_LONGLONG")  = larcv3::kINVALID_LONGLONG;
    m.attr("kINVALID_ULONGLONG") = larcv3::kINVALID_ULONGLONG;
    m.attr("kINVALID_SIZE")      = larcv3::kINVALID_SIZE;
    m.attr("kINVALID_INT")       = larcv3::kINVALID_INT;
    m.attr("kINVALID_UINT")      = larcv3::kINVALID_UINT;
    m.attr("kINVALID_SHORT")     = larcv3::kINVALID_SHORT;
    m.attr("kINVALID_USHORT")    = larcv3::kINVALID_USHORT;
    m.attr("kINVALID_FLOAT")     = larcv3::kINVALID_FLOAT;
    m.attr("kINVALID_DOUBLE")    = larcv3::kINVALID_DOUBLE;
    m.attr("kINVALID_LONG")      = larcv3::kINVALID_LONG;
}
