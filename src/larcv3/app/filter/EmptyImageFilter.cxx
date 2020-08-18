#ifndef __LARCV3FILTER_EMPTYIMAGEFILTER_CXX__
#define __LARCV3FILTER_EMPTYIMAGEFILTER_CXX__

#include "EmptyImageFilter.h"
#include "larcv3/core/dataformat/EventTensor.h"
namespace larcv3 {

  static EmptyImageFilterProcessFactory __global_EmptyImageFilterProcessFactory__;

  EmptyImageFilter::EmptyImageFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void EmptyImageFilter::configure(const json& cfg)
  {
    config = this -> default_config();
    config = augment_default_config(config, cfg);
  }

  void EmptyImageFilter::initialize()
  {}

  bool EmptyImageFilter::process(IOManager& mgr)
  {

    auto const& ev_image = mgr.get_data<larcv3::EventTensor2D>(config["ImageProducer"].get<std::string>());
    if(ev_image.image2d_array().empty()) return false;
    return true;
  }

  void EmptyImageFilter::finalize()
  {}

}
#endif
