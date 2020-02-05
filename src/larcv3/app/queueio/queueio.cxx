#include "queueio.h"


void init_queueio(pybind11::module m){



  pybind11::enum_<larcv3::BatchDataType_t> batchdatatype_t(m,"BatchDataType_t");
  batchdatatype_t.value("kBatchDataUnknown",   larcv3::BatchDataType_t::kBatchDataUnknown);
  batchdatatype_t.value("kBatchDataChar",      larcv3::BatchDataType_t::kBatchDataChar);
  batchdatatype_t.value("kBatchDataShort",     larcv3::BatchDataType_t::kBatchDataShort);
  batchdatatype_t.value("kBatchDataInt",       larcv3::BatchDataType_t::kBatchDataInt);
  batchdatatype_t.value("kBatchDataFloat",     larcv3::BatchDataType_t::kBatchDataFloat);
  batchdatatype_t.value("kBatchDataDouble",    larcv3::BatchDataType_t::kBatchDataDouble);
  batchdatatype_t.value("kBatchDataString",    larcv3::BatchDataType_t::kBatchDataString);
  batchdatatype_t.export_values();

  m.def("BatchDataTypeName", &larcv3::BatchDataTypeName); 


  pybind11::enum_<larcv3::BatchDataState_t> batchdatastate_t(m,"BatchDataState_t");
  batchdatastate_t.value("kBatchStateUnknown",  larcv3::BatchDataState_t::kBatchStateUnknown);
  batchdatastate_t.value("kBatchStateEmpty",    larcv3::BatchDataState_t::kBatchStateEmpty);
  batchdatastate_t.value("kBatchStateFilling",  larcv3::BatchDataState_t::kBatchStateFilling);
  batchdatastate_t.value("kBatchStateFilled",   larcv3::BatchDataState_t::kBatchStateFilled);
  batchdatastate_t.value("kBatchStateReleased", larcv3::BatchDataState_t::kBatchStateReleased);
  batchdatastate_t.export_values();


  init_batchdata(m);
  init_batchdataqueue(m);
  init_batchdataqueuefactory(m);
  init_queueprocessor(m);
}
