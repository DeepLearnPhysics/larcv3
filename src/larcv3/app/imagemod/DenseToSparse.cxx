#ifndef __DENSETOSPARSE_CXX__
#define __DENSETOSPARSE_CXX__

#include "DenseToSparse.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

#include <iomanip>

namespace larcv3 {

static DenseToSparseProcessFactory __global_DenseToSparseProcessFactory__;


DenseToSparse::DenseToSparse(const std::string name)
    : ProcessBase(name) {}



void DenseToSparse::configure(const json& cfg) {

  std::cout << std::setw(4) << "cfg: " << cfg <<std::endl;
  config = this -> default_config();
  std::cout << std::setw(4) << "config: " << config <<std::endl;

  config = augment_default_config(config, cfg);

}


void DenseToSparse::initialize() {}


bool DenseToSparse::process(IOManager& mgr) {

  //We apply thresholding for each projection id.

  // std::cout << "Enter DenseToSparse::process " << std::endl;
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();
  auto const& ref_producer    = config["ReferenceProducer"].get<std::string>();


  if (product == "image2d")
    process_product<larcv3::EventTensor2D, larcv3::EventSparseTensor2D>(mgr, producer, output_producer, ref_producer);
  else if (product == "tensor3d")
    process_product<larcv3::EventTensor3D, larcv3::EventSparseTensor3D>(mgr, producer, output_producer, ref_producer);
  else{
    LARCV_CRITICAL() << "Can't convert product " << product << " to a sparse form " << std::endl;
    throw larbys();
  }

  return true;
}

template <class dataproduct_in, class dataproduct_out>
bool DenseToSparse::process_product(
    IOManager& mgr,
    std::string producer,
    std::string output_producer,
    std::string ref_producer){

  auto const & ev_input  = mgr.template get_data<dataproduct_in>(producer);
  auto       & ev_output = mgr.template get_data<dataproduct_out>(output_producer);

  dataproduct_out ev_ref;

  bool use_ref_data = false;
  if (!ref_producer.empty()){
    ev_ref = mgr.template get_data<dataproduct_out>(ref_producer);
    use_ref_data = true;
  }

  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    auto object = ev_input.as_vector().at(i);
    auto meta = object.meta();

    // Create a voxel set and fill it with all the non-zero tensors
    VoxelSet vs;
    if (use_ref_data){
      auto const& ref_indexes = ev_ref.at(i).indexes_vec();

#ifdef LARCV_OPENMP
#pragma omp parallel for shared(object)
#endif
      for (auto const & index : ref_indexes){
        if (object.as_vector().at(index) != 0.0) vs.emplace(index, object.as_vector().at(index),true);
      }
    }
    else{
#ifdef LARCV_OPENMP
#pragma omp parallel for shared(object)
#endif
      for (size_t index = 0; index < object.as_vector().size(); index ++){
        if (object.as_vector().at(index) != 0.0) vs.emplace(index, object.as_vector().at(index),true);
      }
    }


    ev_output.emplace(std::move(vs), std::move(meta));
  }

  return true;
}



void DenseToSparse::finalize() {}

} //namespace larcv3


#include <pybind11/stl.h>



void init_dense_to_sparse(pybind11::module m){


  using Class = larcv3::DenseToSparse;
  pybind11::class_<Class> DenseToSparse(m, "DenseToSparse");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());


  DenseToSparse.doc() = R"pbdoc(
      Convert a Dense Product to it's corresponding sparse product by removing all 0 elements.
    )pbdoc";


  DenseToSparse.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "DenseToSparse");

  DenseToSparse.def("default_config", &Class::default_config);
}

#endif
