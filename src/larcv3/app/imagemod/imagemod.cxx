#include "imagemod.h"


void init_imagemode(pybind11::module m){

  init_downsample(m);
  init_batchdataqueue(m);
  init_batchdataqueuefactory(m);
  init_queueprocessor(m);
}
