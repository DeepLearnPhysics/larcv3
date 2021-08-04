#ifndef __THRESHOLD_CXX__
#define __THRESHOLD_CXX__

#include "Threshold.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

static ThresholdProcessFactory
    __global_ThresholdProcessFactory__;


Threshold::Threshold(const std::string name)
    : ProcessBase(name) {}

void Threshold::configure(const json& cfg) {

  config = this -> default_config();
  config = augment_default_config(config, cfg);

  // We're really just verifying that everything is a filled
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  if (producer.empty()) {
    LARCV_CRITICAL() << "Must specify a producer" << std::endl;
    throw larbys();
  }
  if (product.empty()) {
    LARCV_CRITICAL() << "Must specify a Product" << std::endl;
    throw larbys();
  }
  if (output_producer.empty()) {
    LARCV_CRITICAL() << "Must specify an OutputProducer" << std::endl;
    throw larbys();
  }

}

void Threshold::initialize() {}

bool Threshold::process(IOManager& mgr) {

  // For the process loop, we go through the product/producer/output producer suite.
  // For each one, we apply thresholding for each projection id.

  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();
  auto const& threshold_v     = config["Threshold"].get<std::vector<float>>();

  if (product == "sparse2d") 
    process_sparse_product<larcv3::EventSparseTensor2D>(mgr, producer, output_producer,threshold_v);
  else if (product == "sparse3d")
    process_sparse_product<larcv3::EventSparseTensor3D>(mgr, producer, output_producer,threshold_v);
  else if (product == "tensor1d")
    process_dense_product<larcv3::EventTensor1D>(mgr, producer, output_producer,threshold_v);
  else if (product == "tensor2d")
    process_dense_product<larcv3::EventTensor2D>(mgr, producer, output_producer,threshold_v);
  else if (product == "tensor3d")
    process_dense_product<larcv3::EventTensor3D>(mgr, producer, output_producer,threshold_v);
  else if (product == "tensor4d")
    process_dense_product<larcv3::EventTensor4D>(mgr, producer, output_producer,threshold_v);
  else if (product == "cluster2d")
    process_sparse_product<EventSparseCluster2D>(mgr, producer, output_producer, threshold_v);
  else if (product == "cluster3d")
    process_sparse_product<EventSparseCluster3D>(mgr, producer, output_producer, threshold_v);
  else{
    LARCV_CRITICAL() << "Encountered unsupported product for threshold: " << product << std::endl;
    throw larbys();
  }
    

  // std::cout << "Exit Threshold::process " << std::endl;

  return true;
}

template< class dataproduct>
bool Threshold::process_sparse_product(IOManager& mgr, std::string producer, 
                                      std::string output_producer,
                                      const std::vector<float> & threshold_v){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto sparse_object(ev_input.as_vector().at(i));
    float threshold;
    if ( threshold_v.size() == 1){
      threshold = threshold_v[0];
    }
    else if(i < threshold_v.size() ){
      threshold = threshold_v[i];
    }
    else{
      LARCV_CRITICAL() << "Threshold_v must be as long as the number of projection IDs!" << std::endl;
      throw larbys();
    }

    sparse_object.threshold_min(threshold);
    ev_output.emplace(std::move(sparse_object));

  }
  return true;

}

template< class dataproduct>
bool Threshold::process_dense_product(IOManager& mgr, std::string producer, 
                                       std::string output_producer,
                                       const std::vector<float> & threshold_v){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto sparse_object(ev_input.as_vector().at(i));
    float threshold;
    if ( threshold_v.size() == 1){
      threshold = threshold_v[0];
    }
    else if(i < threshold_v.size() ){
      threshold = threshold_v[i];
    }
    else{
      LARCV_CRITICAL() << "Threshold_v must be as long as the number of projection IDs!" << std::endl;
      throw larbys();
    }

    sparse_object.threshold(threshold, 0.0);
    ev_output.emplace(std::move(sparse_object));

  }
  return true;

}

void Threshold::finalize() {}

}


#include <pybind11/stl.h>



void init_threshold(pybind11::module m){


  using Class = larcv3::Threshold;
  pybind11::class_<Class> threshold(m, "Threshold");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  threshold.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "Threshold");

  threshold.def("default_config", &Class::default_config);
}


#endif
