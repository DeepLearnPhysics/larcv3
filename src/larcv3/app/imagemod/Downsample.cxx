#ifndef __DOWNSAMPLE_CXX__
#define __DOWNSAMPLE_CXX__

#include "Downsample.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"
#include "larcv3/core/dataformat/EventBBox.h"

namespace larcv3 {

static DownsampleProcessFactory __global_DownsampleProcessFactory__;

 
Downsample::Downsample(const std::string name)
    : ProcessBase(name) {}


 
void Downsample::configure(const json& cfg) {

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

 
void Downsample::initialize() {}

 
bool Downsample::process(IOManager& mgr) {

  //We apply thresholding for each projection id.

  // std::cout << "Enter Downsample::process " << std::endl;
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  auto const& pool            = config["PoolType"].get<PoolType_t>();
  auto const& downsample      = config["Downsample"].get<int>();


  if (product == "sparse2d") 
    process_data_product<larcv3::EventSparseTensor2D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "sparse3d")
    process_data_product<larcv3::EventSparseTensor3D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "tensor1d")
    process_data_product<larcv3::EventTensor1D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "tensor2d")
    process_data_product<larcv3::EventTensor2D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "tensor3d")
    process_data_product<larcv3::EventTensor3D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "tensor4d")
    process_data_product<larcv3::EventTensor4D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "bbox2d")
    process_data_product<larcv3::EventBBox2D>(mgr, producer, output_producer, downsample, pool);
  else if (product == "bbox3d")
    process_data_product<larcv3::EventBBox3D>(mgr, producer, output_producer, downsample, pool);
  else{
    LARCV_CRITICAL() << "Can't apply downsampling to product " << product << std::endl;
    throw larbys();
  }

  return true;
}

template< class dataproduct>
bool Downsample::process_data_product(IOManager& mgr, std::string producer, 
                                      std::string output_producer,
                                      int downsample, PoolType_t pool){

  auto const & ev_input  = mgr.template get_data<dataproduct>(producer);
  auto       & ev_output = mgr.template get_data<dataproduct>(output_producer);

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto object = ev_input.as_vector().at(i);
    auto compressed = object.compress(downsample, larcv3::PoolType_t(pool));
    ev_output.emplace(std::move(compressed));
  }

  return true;
}


 
void Downsample::finalize() {}

} //namespace larcv3


#include <pybind11/stl.h>



void init_downsample(pybind11::module m){


  using Class = larcv3::Downsample;
  pybind11::class_<Class> downsample(m, "Downsample");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  downsample.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "Downsample");

  downsample.def("default_config", &Class::default_config);
}

#endif






