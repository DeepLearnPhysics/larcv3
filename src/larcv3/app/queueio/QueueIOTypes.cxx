#ifndef __LARCV3THREADIO_THREADIOTYPES_CXX
#define __LARCV3THREADIO_THREADIOTYPES_CXX

#include "QueueIOTypes.h"

namespace larcv3{

  std::string BatchDataTypeName(BatchDataType_t type) {
    switch(type) {
    case BatchDataType_t::kBatchDataChar:
      return std::string("char");
    case BatchDataType_t::kBatchDataShort:
      return std::string("short");
    case BatchDataType_t::kBatchDataInt:
      return std::string("int");
    case BatchDataType_t::kBatchDataFloat:
      return std::string("float32");
    case BatchDataType_t::kBatchDataDouble:
      return std::string("float64");
    case BatchDataType_t::kBatchDataString:
      return std::string("string");
    case BatchDataType_t::kBatchDataParticle:
      return std::string("particle");
    default:
      return std::string("");
    }
  }
}

#endif
