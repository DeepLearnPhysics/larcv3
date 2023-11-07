#ifndef __NORMALIZE_CXX__
#define __NORMALIZE_CXX__

#include "Normalize.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

static NormalizeProcessFactory
    __global_NormalizeProcessFactory__;


Normalize::Normalize(const std::string name)
    : ProcessBase(name) {}

void Normalize::configure(const json& cfg) {

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

void Normalize::initialize() {}

bool Normalize::process(IOManager& mgr) {

  // For the process loop, we go through the product/producer/output producer suite.
  // For each one, we apply normalizeing for each projection id.

  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();
  auto const& mean            = config["Mean"].get<float>();
  auto const& std             = config["Std"].get<float>();

  if (product == "sparse2d") 
    process_sparse_product<larcv3::EventSparseTensor2D>(mgr, producer, output_producer,mean, std);
  else if (product == "sparse3d")
    process_sparse_product<larcv3::EventSparseTensor3D>(mgr, producer, output_producer,mean, std);
  else if (product == "tensor1d")
    process_dense_product<larcv3::EventTensor1D>(mgr, producer, output_producer,mean, std);
  else if (product == "tensor2d")
    process_dense_product<larcv3::EventTensor2D>(mgr, producer, output_producer,mean, std);
  else if (product == "tensor3d")
    process_dense_product<larcv3::EventTensor3D>(mgr, producer, output_producer,mean, std);
  else if (product == "tensor4d")
    process_dense_product<larcv3::EventTensor4D>(mgr, producer, output_producer,mean, std);
  // else if (product == "cluster2d")
  //   process_sparse_product<EventSparseCluster2D>(mgr, producer, output_producer, mean, std);
  // else if (product == "cluster3d")
  //   process_sparse_product<EventSparseCluster3D>(mgr, producer, output_producer, mean, std);
  else{
    LARCV_CRITICAL() << "Encountered unsupported product for normalize: " << product << std::endl;
    throw larbys();
  }
    

  // std::cout << "Exit Normalize::process " << std::endl;

  return true;
}

template< class dataproduct>
bool Normalize::process_sparse_product(IOManager& mgr, std::string producer, 
                                      std::string output_producer,
                                      float target_mean,
                                      float target_std){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto sparse_object(ev_input.as_vector().at(i));

    // Create a new output object
    auto output = sparse_object;
    output.clear_data();

    float current_mean(0.0);
    float current_std(0.0);
    size_t n = sparse_object.size();
    // Calculate the mean and std with a pass through the data:
    for (auto & voxel_value : sparse_object.values_vec() ){
      current_mean += voxel_value;
      current_std += voxel_value*voxel_value;
    }
    // Finalize the std and mean:
    current_std = (current_std - (current_mean*current_mean ) / n ) / (n-1);
    current_std = sqrt(current_std);
    current_mean = current_mean / n ;

    // To shift the distribution, we can do the following steps:
    // vals -> vals - current mean (shift to zero mean)
    // vals -> vals * (target_std / current_std)
    // vals -> vals + target_mean

    // Or, all together, looks like this:
    // vals -> (vals - current_mean)*(target_std / current_std) + target_mean
    // vals -> a*vals + b
    // where a = (target_std / current_std) and b = target_mean - current_mean * a

    float scale = target_std / current_std;
    float shift = target_mean - current_mean * scale;
    for (auto & voxel : sparse_object.as_vector() ){

      output.add(Voxel(voxel.id(), scale*voxel.value() + shift));
    }

    ev_output.emplace(std::move(output));

  }
  return true;

}

template< class dataproduct>
bool Normalize::process_dense_product(IOManager& mgr, std::string producer, 
                                       std::string output_producer,
                                       float target_mean,
                                       float target_std){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto dense_object(ev_input.as_vector().at(i));
    auto output(dense_object);
    output.clear_data();

    float current_mean(0.0);
    float current_std(0.0);
    size_t n = dense_object.size();
    // Calculate the mean and std with a pass through the data:
    for (auto & voxel_value : dense_object.as_vector() ){
      current_mean += voxel_value;
      current_std += voxel_value*voxel_value ;
    }
    // Finalize the std and mean:
    current_std = (current_std - (current_mean*current_mean ) / n ) / (n-1);
    current_std = sqrt(current_std);
    current_mean = current_mean / n ;

    float scale = target_std / current_std;
    float shift = target_mean - current_mean * scale;
    for (size_t i = 0; i < n; i ++){

      output.set_pixel(i, scale*dense_object.pixel(i) + shift);
    }

    ev_output.emplace(std::move(output));

  }
  return true;

}

void Normalize::finalize() {}

}


#include <pybind11/stl.h>



void init_normalize(pybind11::module m){


  using Class = larcv3::Normalize;
  pybind11::class_<Class> normalize(m, "Normalize");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  normalize.doc() = R"pbdoc(
      Process that applies a normalize to specified products.  Output product is 
      the same as input product.  Can be applied to dense tensors, sparse tensors, 
      or sparse clusters.
    )pbdoc";

  normalize.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "Normalize");

  normalize.def("default_config", &Class::default_config);
}


#endif
