#ifndef __DOWNSAMPLE_CXX__
#define __DOWNSAMPLE_CXX__

#include "Downsample.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

static DownsampleProcessFactory
    __global_DownsampleProcessFactory__;


Downsample::Downsample(const std::string name)
    : ProcessBase(name) {}


void Downsample::configure(const json& cfg) {

  config = this -> default_config();
  config = augment_default_config(config, cfg);

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


    // The VoxelSet and VoxelSetArray both have thresholding functions.

    // It is annoying, but it still has to be split:

    if (product == "sparse2d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventSparseTensor2D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventSparseTensor2D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        auto const & sparse_object = ev_input.sparse_tensor(i);
        SparseTensor2D compressed = sparse_object.compress(downsample, larcv3::PoolType_t(pool));
        ev_output.emplace(std::move(compressed));
      }
    }
    else if (product == "sparse3d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventSparseTensor3D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventSparseTensor3D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        auto sparse_object = ev_input.as_vector().at(i);
        SparseTensor3D compressed = sparse_object.compress(downsample, larcv3::PoolType_t(pool));
        ev_output.emplace(std::move(compressed));

      }
    }
    else if (product == "tensor1d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventTensor1D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventTensor1D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        auto object = ev_input.as_vector().at(i);
        Tensor1D compressed = object.compress(downsample, larcv3::PoolType_t(pool));
        ev_output.emplace(std::move(compressed));
      }
    }
    else if (product == "tensor2d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventTensor2D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventTensor2D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        auto object = ev_input.as_vector().at(i);
        Tensor2D compressed = object.compress(downsample, larcv3::PoolType_t(pool));
        ev_output.emplace(std::move(compressed));
      }
    }
    else if (product == "tensor3d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventTensor3D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventTensor3D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        auto object = ev_input.as_vector().at(i);
        Tensor3D compressed = object.compress(downsample, larcv3::PoolType_t(pool));
        ev_output.emplace(std::move(compressed));
      }
    }
    else if (product == "tensor4d"){
      auto const & ev_input  = mgr.get_data<larcv3::EventTensor4D>(producer);
      auto       & ev_output = mgr.get_data<larcv3::EventTensor4D>(output_producer);

      for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
        auto object = ev_input.as_vector().at(i);
        Tensor4D compressed = object.compress(downsample, larcv3::PoolType_t(pool));
        ev_output.emplace(std::move(compressed));
      }
    }
    else{
      LARCV_CRITICAL() << "Can't apply downsampling to product " << product << std::endl;
      throw larbys();
    }
  }
  // std::cout << "Exit Downsample::process " << std::endl;

  return true;
}

void Downsample::finalize() {}


#include <pybind11/stl.h>

void init_downsample(pybind11::module m){

  using Class = larcv3::Downsample;
  pybind11::class_<Class> queueproc(m, "Downsample");

  queueproc.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "Downsample");

  queueproc.def("default_config", &Class::default_config);



}


}
#endif
