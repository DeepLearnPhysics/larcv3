#ifndef __SPARSETODENSE_CXX__
#define __SPARSETODENSE_CXX__

#include "SparseToDense.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

static SparseToDenseProcessFactory __global_SparseToDenseProcessFactory__;

 
SparseToDense::SparseToDense(const std::string name)
    : ProcessBase(name) {}


 
void SparseToDense::configure(const json& cfg) {

  config = this -> default_config();
  config = augment_default_config(config, cfg);

}

 
void SparseToDense::initialize() {}

 
bool SparseToDense::process(IOManager& mgr) {

  //We apply thresholding for each projection id.

  // std::cout << "Enter SparseToDense::process " << std::endl;
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();


  if (product == "sparse2d") 
    process_product<larcv3::EventSparseTensor2D, larcv3::EventTensor2D>(mgr, producer, output_producer);
  else if (product == "sparse3d")
    process_product<larcv3::EventSparseTensor3D, larcv3::EventTensor3D>(mgr, producer, output_producer);
  else{
    LARCV_CRITICAL() << "Can't convert product " << product << " to a dense form " << std::endl;
    throw larbys();
  }

  return true;
}

template <class dataproduct_in, class dataproduct_out>
bool SparseToDense::process_product(
    IOManager& mgr,
    std::string producer, 
    std::string output_producer){

  auto const & ev_input  = mgr.template get_data<dataproduct_in>(producer);
  auto       & ev_output = mgr.template get_data<dataproduct_out>(output_producer);

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto object = ev_input.as_vector().at(i);
    auto dense = object.to_tensor();
    ev_output.emplace(std::move(dense));
  }

  return true;
}


 
void SparseToDense::finalize() {}

} //namespace larcv3


#include <pybind11/stl.h>



void init_sparse_to_dense(pybind11::module m){


  using Class = larcv3::SparseToDense;
  pybind11::class_<Class> SparseToDense(m, "SparseToDense");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  SparseToDense.doc() = R"pbdoc(
      Convert Sparse Products to their corresponding dense products.
    )pbdoc";



  SparseToDense.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "SparseToDense");

  SparseToDense.def("default_config", &Class::default_config);
}

#endif






