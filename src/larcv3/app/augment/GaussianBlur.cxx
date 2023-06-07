#ifndef __GAUSSIANBLUR_CXX__
#define __GAUSSIANBLUR_CXX__

#include "GaussianBlur.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"

#include "random"

namespace larcv3 {

static GaussianBlurProcessFactory
    __global_GaussianBlurProcessFactory__;


GaussianBlur::GaussianBlur(const std::string name)
    : ProcessBase(name) {}

void GaussianBlur::configure(const json& cfg) {

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

void GaussianBlur::initialize() {}

bool GaussianBlur::process(IOManager& mgr) {



  // For the process loop, we go through the product/producer/output producer suite.
  // For each one, we apply GaussianBluring for each projection id.

  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();
  auto const& sigma           = config["Sigma"].get<float>();

  // Set up the random engine:
  
  std::default_random_engine generator;
  generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
  
  std::normal_distribution<float> distribution(0.0,sigma);


  if (product == "sparse2d") 
    process_sparse_product<larcv3::EventSparseTensor2D>(mgr, producer, output_producer,distribution, generator);
  else if (product == "sparse3d")
    process_sparse_product<larcv3::EventSparseTensor3D>(mgr, producer, output_producer,distribution, generator);
  else if (product == "tensor1d")
    process_dense_product<larcv3::EventTensor1D>(mgr, producer, output_producer,distribution, generator);
  else if (product == "tensor2d")
    process_dense_product<larcv3::EventTensor2D>(mgr, producer, output_producer,distribution, generator);
  else if (product == "tensor3d")
    process_dense_product<larcv3::EventTensor3D>(mgr, producer, output_producer,distribution, generator);
  else if (product == "tensor4d")
    process_dense_product<larcv3::EventTensor4D>(mgr, producer, output_producer,distribution, generator);
  // else if (product == "cluster2d")
  //   process_sparse_product<EventSparseCluster2D>(mgr, producer, output_producer, distribution);
  // else if (product == "cluster3d")
  //   process_sparse_product<EventSparseCluster3D>(mgr, producer, output_producer, distribution);
  else{
    LARCV_CRITICAL() << "Encountered unsupported product for GaussianBlur: " << product << std::endl;
    throw larbys();
  }
    

  // std::cout << "Exit GaussianBlur::process " << std::endl;

  return true;
}

template< class dataproduct>
bool GaussianBlur::process_sparse_product(IOManager& mgr, std::string producer, 
                                          std::string output_producer,
                                          std::normal_distribution<float> distribution,
                                          std::default_random_engine generator){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);


  // Loop over the projection IDs:
  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    
    // The input object on this projection:

    auto & input = ev_input.as_vector().at(i);
    // Create a new output object
    auto output = input;
    output.clear_data();
    // Set the meta:

    for (auto & voxel : input.as_vector()){
      // generate a new number:
      auto kick = distribution(generator);
      output.add(Voxel(voxel.id(), voxel.value() + kick));
    }

    ev_output.emplace(std::move(output));

  }

  return true;

}

template< class dataproduct>
bool GaussianBlur::process_dense_product(IOManager& mgr, std::string producer, 
                                         std::string output_producer,
                                         std::normal_distribution<float> distribution,
                                         std::default_random_engine generator){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);

  // Loop over the projection IDs:
  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    // # Get the reference object:
    auto & input = ev_input.as_vector().at(i);
    auto output_object(input);

    // Loop over the entire image:
    for (size_t index = 0; index < input.size(); index ++){
      auto kick = distribution(generator);
      output_object.set_pixel(index, input.pixel(index) + kick);
    }

    ev_output.emplace(std::move(output_object));

  }
  return true;

}

void GaussianBlur::finalize() {}

}


#include <pybind11/stl.h>



void init_gaussian_blur(pybind11::module m){


  using Class = larcv3::GaussianBlur;
  pybind11::class_<Class> GaussianBlur(m, "GaussianBlur");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  GaussianBlur.doc() = R"pbdoc(
      Process that applies a GaussianBlur to specified products.  Output product is 
      the same as input product.  Can be applied to dense tensors, sparse tensors, 
      but not sparse clusters.
    )pbdoc";

  GaussianBlur.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "GaussianBlur");

  GaussianBlur.def("default_config", &Class::default_config);
}


#endif
