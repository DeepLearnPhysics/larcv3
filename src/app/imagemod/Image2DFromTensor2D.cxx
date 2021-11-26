#ifndef __LARCV_IMAGE2DFROMTENSOR2D_CXX__
#define __LARCV_IMAGE2DFROMTENSOR2D_CXX__

#include "Image2DFromTensor2D.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventTensor.h"
namespace larcv3 {

  static Image2DFromTensor2DProcessFactory __global_Image2DFromTensor2DProcessFactory__;

  Image2DFromTensor2D::Image2DFromTensor2D(const std::string name)
    : ProcessBase(name)
  {}

  void Image2DFromTensor2D::configure_labels(const PSet& cfg)
  {
    LARCV_DEBUG() << "start" << std::endl;
    _tensor2d_producer_v.clear();
    _output_producer_v.clear();
    _tensor2d_producer_v = cfg.get<std::vector<std::string> >("Tensor2DProducerList", _tensor2d_producer_v);
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_tensor2d_producer_v.empty()) {
      auto tensor2d_producer = cfg.get<std::string>("Tensor2DProducer", "");
      auto output_producer    = cfg.get<std::string>("OutputProducer", "");
      if (!tensor2d_producer.empty()) {
        _tensor2d_producer_v.push_back(tensor2d_producer);
        if (output_producer.empty()) output_producer = tensor2d_producer + "_tensor2d";
        _output_producer_v.push_back(output_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_tensor2d_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor2DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
    for (size_t producer_index = 0; producer_index < _tensor2d_producer_v.size(); ++producer_index) {
      auto const& tensor2d_producer = _tensor2d_producer_v[producer_index];
      auto const& output_producer = _output_producer_v[producer_index];
      LARCV_INFO() << "Creating Imge2D (" << output_producer << ")"
      << " from Tensor2D (" << tensor2d_producer << ")" << std::endl;
    }
    LARCV_DEBUG() << "end" << std::endl;
  }

  void Image2DFromTensor2D::configure(const PSet& cfg)
  {
    configure_labels(cfg);
    _type_pi_v = cfg.get<std::vector<unsigned short> >("TypePIList", _type_pi_v);
    if (_type_pi_v.empty()) {
      auto type_pi = cfg.get<unsigned short>("TypePI", (unsigned short)(kPITypeInputVoxel));
      _type_pi_v.resize(_tensor2d_producer_v.size(), type_pi);
    } else if (_type_pi_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "TypePIList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _fixed_pi_v = cfg.get<std::vector<float> >("FixedPIList", _fixed_pi_v);
    if (_fixed_pi_v.empty()) {
      auto fixed_pi = cfg.get<float>("FixedPI", 1.);
      _fixed_pi_v.resize(_tensor2d_producer_v.size(), fixed_pi);
    } else if (_fixed_pi_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "FixedPIList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _base_pi_v = cfg.get<std::vector<float> >("BasePIList", _base_pi_v);
    if (_base_pi_v.empty()) {
      auto base_pi = cfg.get<float>("BasePI", 0.);
      _base_pi_v.resize(_tensor2d_producer_v.size(), base_pi);
    } else if (_base_pi_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "BasePIList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
  }

  void Image2DFromTensor2D::initialize() {}

  bool Image2DFromTensor2D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _tensor2d_producer_v.size(); ++producer_index) {
      auto const& tensor2d_producer = _tensor2d_producer_v[producer_index];
      auto const& output_producer = _output_producer_v[producer_index];
      auto const& fixed_pi = _fixed_pi_v[producer_index];
      auto const& base_pi  = _base_pi_v[producer_index];
      auto const type_pi = (PIType_t)(_type_pi_v[producer_index]);


      auto const& ev_tensor2d = mgr.get_data<larcv3::EventSparseTensor2D>(tensor2d_producer);
      auto& ev_out_image = mgr.get_data<larcv3::EventTensor2D>(output_producer);


      for (auto const& tensor2d_v : ev_tensor2d.as_vector()) {


        auto const& meta = tensor2d_v.meta();

        std::vector<float> img_data(meta.total_voxels(),base_pi);

        for (auto const& vox : tensor2d_v.as_vector()) {
          float val = fixed_pi;
          switch (type_pi) {
          case PIType_t::kPITypeFixedPI:
            break;
          case PIType_t::kPITypeInputVoxel:
            val = std::max(img_data[vox.id()], vox.value());
            break;
          case PIType_t::kPITypeUndefined:
            throw larbys("PITypeUndefined not supported!");
            break;
          }
          img_data[vox.id()] = val;
        }
        larcv3::Image2D img2d(std::move(meta), std::move(img_data));
        ev_out_image.emplace(std::move(img2d));
      }
  
    }


    return true;
  }

  void Image2DFromTensor2D::finalize()
  {}

}
#endif
