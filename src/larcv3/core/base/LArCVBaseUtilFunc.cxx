#ifndef __LARCV3BASE_UTILFUNC_CXX__
#define __LARCV3BASE_UTILFUNC_CXX__

#include "larcv3/core/base/LArCVBaseUtilFunc.h"
#include <sstream>
#include <fstream>
namespace larcv3 {


  bool mpi_enabled(){
#ifdef LARCV_MPI
    return true;
#else
    return false;
#endif
  }

}

#endif
