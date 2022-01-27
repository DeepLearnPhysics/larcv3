#ifndef __LARCV3FILTER_EMPTYTENSORFILTER_CXX__
#define __LARCV3FILTER_EMPTYTENSORFILTER_CXX__

#include "EmptyTensorFilter.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventTensor.h"
namespace larcv3 {

  static EmptyTensorFilterProcessFactory __global_EmptyTensorFilterProcessFactory__;

  EmptyTensorFilter::EmptyTensorFilter(const std::string name)
    : ProcessBase(name)
  {}

  // void EmptyTensorFilter::configure_labels(const json& cfg)
  // {
  //   _tensor2d_producer_v.clear();
  //   _tensor3d_producer_v.clear();

  //   _tensor2d_producer_v = cfg.get<std::vector<std::string> >("Tensor2DProducerList", _tensor2d_producer_v);
  //   _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducerList", _tensor3d_producer_v);

  //   if (_tensor2d_producer_v.empty()) {
  //     auto tensor2d_producer = cfg.get<std::string>("Tensor2DProducer", "");
  //     if (!tensor2d_producer.empty()) {
  //       _tensor2d_producer_v.push_back(tensor2d_producer);
  //     }
  //   }

  //   if (_tensor3d_producer_v.empty()) {
  //     auto tensor3d_producer = cfg.get<std::string>("Tensor3DProducer", "");
  //     if (!tensor3d_producer.empty()) {
  //       _tensor3d_producer_v.push_back(tensor3d_producer);
  //     }
  //   }
  // }

  void EmptyTensorFilter::configure(const json& cfg)
  {

    config = this -> default_config();
    config = augment_default_config(config, cfg);

    // assert that all lengths are the same:

    auto size = config["TensorProducer"].get<std::vector<std::string>>().size();

    std::vector<std::string> keys = {"TensorType", "MinVoxelCount", "MinVoxelValue"};

    if ( config["TensorType"].get<std::vector<std::string>>().size() != size){
      LARCV_CRITICAL() << "TensorType" << " size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
    if ( config["MinVoxelCount"].get<std::vector<int>>().size() != size){
      LARCV_CRITICAL() << "MinVoxelCount" << " size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
    if ( config["MinVoxelValue"].get<std::vector<float>>().size() != size){
      LARCV_CRITICAL() << "MinVoxelValue" << " size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

  }

  void EmptyTensorFilter::initialize()
  {}

  bool EmptyTensorFilter::process(IOManager& mgr)
  {

    auto producers = config["TensorProducer"].get<std::vector<std::string>>();
    auto products  = config["TensorProduct"].get<std::vector<std::string>>();
    auto values    = config["MinVoxelCount"].get<std::vector<int>>();
    auto counts    = config["MinVoxelValue"].get<std::vector<float>>();

    for (size_t index = 0; index < producers.size(); index ++){

      bool filter_status = true;

      if (products[index] == "sparse2d") {
        filter_status = process_sparse<2>(mgr, producers[index], values[index], counts[index]);
      }
      else if (products[index] == "sparse3d"){
        filter_status = process_sparse<3>(mgr, producers[index], values[index], counts[index]);
      }
      else if (products[index] == "tensor1d"){
        filter_status = process_tensor<1>(mgr, producers[index], values[index], counts[index]);
      }
      else if (products[index] == "tensor2d" || products[index] == "image2d"){
        filter_status = process_tensor<2>(mgr, producers[index], values[index], counts[index]);
      }
      else if (products[index] == "tensor3d"){
        filter_status = process_tensor<3>(mgr, producers[index], values[index], counts[index]);
      }
      else if (products[index] == "tensor4d"){
        filter_status = process_tensor<4>(mgr, producers[index], values[index], counts[index]);
      }
      if (! filter_status) return false;
    }

    return true;
  }

  template<size_t dimension>
  bool EmptyTensorFilter::process_tensor(IOManager & mgr, std::string producer, int min_voxel_count, float min_voxel_value){

    // Get the data:
    auto const& event_tensor = mgr.get_data<EventTensor<dimension>>(producer);
    for (auto const& tensor : event_tensor.as_vector()) {
      int ctr = 0;
      for (auto const& vox_value : tensor.as_vector()) {
        if (vox_value < min_voxel_value) continue;
        ctr++;
      }
      if (ctr < min_voxel_count) return false;
    }
    return true;
  }

  template <size_t dimension>
  bool EmptyTensorFilter::process_sparse(IOManager & mgr, std::string producer, int min_voxel_count, float min_voxel_value){

    // Get the data:
    auto const& event_tensor = mgr.get_data<EventSparseTensor<dimension>>(producer);
    for (auto const& tensor : event_tensor.as_vector()) {
      int ctr = 0;
      for (auto const& vox_value : tensor.values_vec()) {
        if (vox_value < min_voxel_value) continue;
        ctr++;
      }
      if (ctr < min_voxel_count) return false;
    }
    return true;

  }

  void EmptyTensorFilter::finalize()
  {}

}
#endif
