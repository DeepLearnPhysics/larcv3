#ifndef __MIRROR_CXX__
#define __MIRROR_CXX__

#include "Mirror.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"

#include "random"

namespace larcv3 {

static MirrorProcessFactory
    __global_MirrorProcessFactory__;


Mirror::Mirror(const std::string name)
    : ProcessBase(name) {}

void Mirror::configure(const json& cfg) {

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

void Mirror::initialize() {}

bool Mirror::process(IOManager& mgr) {

  // For the process loop, we go through the product/producer/output producer suite.
  // For each one, we apply Mirroring for each projection id.

  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();
  auto const& axes            = config["Axes"].get<std::vector<size_t>>();

  // Set up the random engine:
  std::default_random_engine generator;
  generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
  
  auto gen = std::bind(std::uniform_int_distribution<>(0,1), generator);

  
  // The random factor here is to select the dimensions.
  // We pick them in advance and keep them consistent over all projections.

  // Select the mirrored axes here:
  std::vector<uint> mirror_by_axis;
  mirror_by_axis.resize(axes.size());
  for (auto i : axes){
    // Draw a random bool:
    auto rand_int = gen();
    mirror_by_axis[i] = rand_int;
  }

  if (product == "sparse2d") 
    process_sparse_product<larcv3::EventSparseTensor2D>(mgr, producer, output_producer, mirror_by_axis);
  else if (product == "sparse3d")
    process_sparse_product<larcv3::EventSparseTensor3D>(mgr, producer, output_producer, mirror_by_axis);
  else if (product == "tensor1d")
    process_dense_product<larcv3::EventTensor1D>(mgr, producer, output_producer, mirror_by_axis);
  else if (product == "tensor2d")
    process_dense_product<larcv3::EventTensor2D>(mgr, producer, output_producer, mirror_by_axis);
  else if (product == "tensor3d")
    process_dense_product<larcv3::EventTensor3D>(mgr, producer, output_producer, mirror_by_axis);
  else if (product == "tensor4d")
    process_dense_product<larcv3::EventTensor4D>(mgr, producer, output_producer, mirror_by_axis);
  // else if (product == "cluster2d")
  //   process_sparse_product<EventSparseCluster2D>(mgr, producer, output_producer, distribution);
  // else if (product == "cluster3d")
  //   process_sparse_product<EventSparseCluster3D>(mgr, producer, output_producer, distribution);
  else{
    LARCV_CRITICAL() << "Encountered unsupported product for Mirror: " << product << std::endl;
    throw larbys();
  }
    

  // std::cout << "Exit Mirror::process " << std::endl;

  return true;
}

template< class dataproduct>
bool Mirror::process_sparse_product(IOManager& mgr, std::string producer, 
                                          std::string output_producer,
                                          std::vector<uint> mirror_by_axis){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);


  // Loop over the projection IDs:
  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    
    // The input object on this projection:

    auto & input = ev_input.as_vector().at(i);
    // Create a new output object
    auto output = input;
    output.clear_data();

    // A double loop is a silly way to do this.
    // Will fix it if it becomes a bottle neck.
    for (auto & voxel : input.as_vector()){
      
      // Having an if statement in here is slow.
      // So, instead havethe mirror be a 0 or a 1 integer, 
      // with 0 = false and 1 = true
      // and compute new coord along an axis as:
      // new_coord = mirror * number_of_voxels + (1 - mirror ) * original_coord

      // Get the coordinates of this voxel:
      auto coords = output.meta().coordinates(voxel.id());
      for (size_t axis = 0; axis < coords.size(); axis ++ ){
        // if (mirror_by_axis.count(axis) && mirror_by_axis[axis]){
          // Then, flip this coordinate:
          coords[axis] = mirror_by_axis[axis] * output.meta().number_of_voxels(axis) 
                         + (1 - mirror_by_axis[axis]) * coords[axis];
        // }
      }
      auto new_id = output.meta().index(coords);
      output.add(Voxel(new_id, voxel.value()));
    }

    ev_output.emplace(std::move(output));

  }

  return true;

}

template< class dataproduct>
bool Mirror::process_dense_product(IOManager& mgr, std::string producer, 
                                         std::string output_producer,
                                         std::vector<uint> mirror_by_axis){

  auto const ev_input  = mgr.template get_data<dataproduct>(producer);
  auto & ev_output = mgr.template get_data<dataproduct>(output_producer);

  // Loop over the projection IDs:
  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){
    // # Get the reference object:
    auto & input = ev_input.as_vector().at(i);
    auto output_object(input);

    // Loop over the entire image:
    for (size_t index = 0; index < input.size(); index ++){
      
      // Get the coordinates of this voxel:
      auto coords = output_object.meta().coordinates(index);
      for (size_t axis = 0; axis < coords.size(); axis ++ ){
        // if (mirror_by_axis.count(axis) && mirror_by_axis[axis]){
          // Then, flip this coordinate:
          coords[axis] = mirror_by_axis[axis] * output_object.meta().number_of_voxels(axis) 
                         + (1 - mirror_by_axis[axis]) * coords[axis];
        // }
      }
      auto new_id = output_object.meta().index(coords);
      output_object.set_pixel(new_id, input.pixel(index));
    }

    ev_output.emplace(std::move(output_object));

  }
  return true;

}

void Mirror::finalize() {}

}


#include <pybind11/stl.h>



void init_mirror(pybind11::module m){


  using Class = larcv3::Mirror;
  pybind11::class_<Class> Mirror(m, "Mirror");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  Mirror.doc() = R"pbdoc(
      Process that applies a Mirror to specified products.  Output product is 
      the same as input product.  Can be applied to dense tensors, sparse tensors, 
      but not sparse clusters.
    )pbdoc";

  Mirror.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "Mirror");

  Mirror.def("default_config", &Class::default_config);
}


#endif
