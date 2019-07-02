#ifndef __THRESHOLD_CXX__
#define __THRESHOLD_CXX__

#include "Threshold.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

static ThresholdProcessFactory
    __global_ThresholdProcessFactory__;


Threshold::Threshold(const std::string name)
    : ProcessBase(name) {}

void Threshold::configure_labels(const PSet& cfg) {
  _input_producer_v.clear();
  _input_product_v.clear();
  _output_producer_v.clear();
  _input_producer_v
    = cfg.get<std::vector<std::string> >("ProducerList",
                                         _input_producer_v);
  _input_product_v
    = cfg.get<std::vector<std::string> >("ProductList",
                                         _input_product_v);
  _output_producer_v
    = cfg.get<std::vector<std::string> >("OutputProducerList",
                                         _output_producer_v);

  if (_input_producer_v.empty()) {
    auto producer = cfg.get<std::string>("Producer", "");
    auto product = cfg.get<std::string>("Product", "");
    auto output_producer = cfg.get<std::string>("OutputProducer", "");
    if (!producer.empty()) {
      _input_producer_v.push_back(producer);
      if (output_producer.empty())
        output_producer = producer + "_threshold";
      _output_producer_v.push_back(output_producer);
    }
    
    // Look if the product is empty or not.  If not, add it to the product list
    if (!product.empty()){
      _input_product_v.push_back(product);
    }
  }

  // Check if the product list is size 1 or same size as producer list. 
  // if same size, good.
  // if size 1, broadcast.
  // otherwise, exception.

  if (_input_product_v.size() != _input_producer_v.size()){

    if (_input_product_v.size() == 1 && _input_product_v.size() != _input_producer_v.size()){
      // Broadcast to the rest of the spots:
      _input_product_v.resize(_input_producer_v.size());
      for ( size_t i = 0; i < _input_producer_v.size(); i ++){
        _input_product_v[i] = _input_product_v[0];
      }
    }

    else{
      LARCV_CRITICAL() << "Producer and Product must have the "
                        "same array length, or only one product!"
                     << std::endl;
      throw larbys();
    }

  }


  if (_output_producer_v.empty()) {
    _output_producer_v.resize(_input_producer_v.size(), "");
  } else if (_output_producer_v.size() != _input_producer_v.size()) {
    LARCV_CRITICAL() << "Producer and OutputProducer must have the "
                        "same array length!"
                     << std::endl;
    throw larbys();
  }
}

void Threshold::configure(const PSet& cfg) {
  configure_labels(cfg);

  _thresholds_v = cfg.get<std::vector<float> >("ThresholdList", _thresholds_v);
  if (_thresholds_v.empty()) {
    auto threshold = cfg.get<float>("Threshold");
    _thresholds_v.push_back(threshold);
  }

  if (_thresholds_v.empty()) {
    LARCV_CRITICAL() << "Must specify a threshold" << std::endl;
    throw larbys();
  }


}

void Threshold::initialize() {}

bool Threshold::process(IOManager& mgr) {

  // For the process loop, we go through the product/producer/output producer suite.
  // For each one, we apply thresholding for each projection id.

  // std::cout << "Enter Threshold::process " << std::endl;
  for (size_t producer_index = 0; producer_index < _input_producer_v.size();
       ++producer_index) {
    auto const& producer        = _input_producer_v[producer_index];
    auto const& product         = _input_product_v[producer_index];
    auto const& output_producer = _output_producer_v[producer_index];


    // The VoxelSet and VoxelSetArray both have thresholding functions.

    // It is annoying, but it still has to be split:

    if (product == "sparse2d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventSparseTensor2D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventSparseTensor2D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        larcv3::SparseTensor2D sparse_object(ev_input.as_vector().at(i));
        float threshold;
        if ( _thresholds_v.size() == 1){
          threshold = _thresholds_v[0];
        }
        else if(i < _thresholds_v.size() ){
          threshold = _thresholds_v[i];
        }
        else{
          LARCV_CRITICAL() << "Threshold_v must be as long as the number of projection IDs!" << std::endl;
          throw larbys();
        }

        sparse_object.threshold_min(threshold);
        ev_output.emplace(std::move(sparse_object));

      }
    }
    if (product == "sparse3d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventSparseTensor3D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventSparseTensor3D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        larcv3::SparseTensor3D sparse_object(ev_input.as_vector().at(i));
        float threshold;
        if ( _thresholds_v.size() == 1){
          threshold = _thresholds_v[0];
        }
        else if(i < _thresholds_v.size() ){
          threshold = _thresholds_v[i];
        }
        else{
          LARCV_CRITICAL() << "Threshold_v must be as long as the number of projection IDs!" << std::endl;
          throw larbys();
        }

        sparse_object.threshold_min(threshold);
        ev_output.emplace(std::move(sparse_object));

      }
    }
    if (product == "cluster2d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventSparseCluster2D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventSparseCluster2D>(output_producer);


      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        larcv3::SparseCluster2D sparse_object(ev_input.as_vector().at(i));
        float threshold;
        if ( _thresholds_v.size() == 1){
          threshold = _thresholds_v[0];
        }
        else if(i < _thresholds_v.size() ){
          threshold = _thresholds_v[i];
        }
        else{
          LARCV_CRITICAL() << "Threshold_v must be as long as the number of projection IDs!" << std::endl;
          throw larbys();
        }

        sparse_object.threshold_min(threshold);
        ev_output.emplace(std::move(sparse_object));

      }
    }
    if (product == "sparse3d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventSparseCluster3D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventSparseCluster3D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        larcv3::SparseCluster3D sparse_object(ev_input.as_vector().at(i));
        float threshold;
        if ( _thresholds_v.size() == 1){
          threshold = _thresholds_v[0];
        }
        else if(i < _thresholds_v.size() ){
          threshold = _thresholds_v[i];
        }
        else{
          LARCV_CRITICAL() << "Threshold_v must be as long as the number of projection IDs!" << std::endl;
          throw larbys();
        }

        sparse_object.threshold_min(threshold);
        ev_output.emplace(std::move(sparse_object));

      }
    }
    
    
    // for (auto & sparse_object : ev_input.as_vector()){
    //   sparse_object.threshold(_theso)
    // }

    // auto const& threshold = _thresholds_v[producer_index];


    // for (size_t cluster_index = 0; cluster_index < cluster2d_v.size();
    //      ++cluster_index) {
    //   // Create a voxelsetarray for each cluster

    //   auto const& cluster_pixel_2d = cluster2d_v.at(cluster_index);


    //   // Each ClusterPixel2D has a vector of VoxelSets,
    //   // and each VoxelSet has a vector of voxels
    //   //
    //   // Loop over the ClusterPixel2D and copy all voxels
    //   // above thresholds to the new producers

    //   ClusterPixel2D thresholded_cluster_pixel_2d;

    //   // Output meta is the same as new meta
    //   thresholded_cluster_pixel_2d.meta(cluster_pixel_2d.meta());

    //   for (size_t voxel_set_index = 0;
    //        voxel_set_index < cluster_pixel_2d.as_vector().size();
    //        ++voxel_set_index) {
    //     // Look at each voxel set, which allows iteration of voxels
    //     auto const& original_voxel_set =
    //         cluster_pixel_2d.as_vector().at(voxel_set_index);
    //     // Create new voxel sets:
    //     VoxelSet thresholded_voxels;
    //     thresholded_voxels.id(original_voxel_set.id());

    //     for (size_t voxel_index = 0;
    //          voxel_index < original_voxel_set.as_vector().size();
    //          ++voxel_index) {
    //       auto const& voxel = original_voxel_set.as_vector().at(voxel_index);
    //       // std::cout <<"---Voxel value: " << voxel.value() << ", threshold: " << threshold << std::endl;
    //       if (voxel.value() >= threshold) {
    //         // std::cout <<"----above threshold" << std::endl;
    //         // thresholded_voxels.emplace(std::move(voxel), true);
    //         thresholded_voxels.add(voxel);
    //       }
    //     }
    //     // There is now a VoxelSet of thresholded voxels, add it to the voxel
    //     // set:
    //     thresholded_cluster_pixel_2d.emplace(std::move(thresholded_voxels));
    //   }

    //   // Add the ClusterPixel2D to the event data:
    //   ev_cluster2d_output.emplace(std::move(thresholded_cluster_pixel_2d));

    // }

  }
  // std::cout << "Exit Threshold::process " << std::endl;

  return true;
}

void Threshold::finalize() {}
}
#endif
