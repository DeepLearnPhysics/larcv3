#ifndef __LARCV3THREADIO_BATCHFILLERTEMPLATE_CXX__
#define __LARCV3THREADIO_BATCHFILLERTEMPLATE_CXX__

#include "BatchFillerTemplate.h"
#include "larcv3/core/dataformat/Particle.h"
#include <sstream>

namespace larcv3 {

  template<> BatchDataType_t BatchFillerTemplate< short  >::data_type() const     { return BatchDataType_t::kBatchDataShort;  }
  template<> BatchDataType_t BatchFillerTemplate< int    >::data_type() const     { return BatchDataType_t::kBatchDataInt;    }
  template<> BatchDataType_t BatchFillerTemplate< float  >::data_type() const     { return BatchDataType_t::kBatchDataFloat;  }
  template<> BatchDataType_t BatchFillerTemplate< double >::data_type() const     { return BatchDataType_t::kBatchDataDouble; }
  template<> BatchDataType_t BatchFillerTemplate< larcv3::ParticleHolder>::data_type() const { return BatchDataType_t::kBatchDataParticle;}



  template class BatchFillerTemplate<short>;
  template class BatchFillerTemplate<int>;
  template class BatchFillerTemplate<float>;
  template class BatchFillerTemplate<double>;
  template class BatchFillerTemplate<larcv3::ParticleHolder>;



}
#endif
