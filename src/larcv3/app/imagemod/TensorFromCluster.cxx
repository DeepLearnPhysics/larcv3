#ifndef __LARCV_TENSORFROMCLUSTER_CXX__
#define __LARCV_TENSORFROMCLUSTER_CXX__

#include "TensorFromCluster.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"


namespace larcv3 {


static TensorFromClusterProcessFactory
__global_TensorFromClusterProcessFactory__;

TensorFromCluster::TensorFromCluster(const std::string name)
: ProcessBase(name) {}


void TensorFromCluster::configure(const json& cfg) {

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

void TensorFromCluster::initialize() {}

bool TensorFromCluster::process(IOManager& mgr) {
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  if (product == "cluster2d")
    merge_clusters<EventSparseCluster2D, EventSparseTensor2D>(mgr, producer, output_producer);
  else if (product == "cluster3d")
    merge_clusters<EventSparseCluster3D, EventSparseTensor3D>(mgr, producer, output_producer);
  else{
    LARCV_CRITICAL() << "Encountered unsupported product for TensorFromCluster: " << product << std::endl;
    throw larbys();
  }

  return true;
}


template<class dataproduct_in, class dataproduct_out>
bool TensorFromCluster::merge_clusters(IOManager& mgr, std::string producer, std::string output_producer){


  // Get the input and output objects:
  auto const ev_input  = mgr.template get_data<dataproduct_in>(producer);
  auto & ev_output     = mgr.template get_data<dataproduct_out>(output_producer);

  for ( size_t projection_id = 0; 
        projection_id < ev_input.as_vector().size(); 
        projection_id ++ ){
    // Get the clusters for this projection id:
    auto proj_clusters = ev_input.sparse_cluster(projection_id);
    // And, the associated meta:
    auto meta = proj_clusters.meta();
    
    // Get a voxel set to hold the output:
    larcv3::VoxelSet vs;

    // Loop over the clusters:
    for (auto const & cluster : proj_clusters.as_vector()){
      // Loop over the voxels in the cluster:
      for (auto const & vox : cluster.as_vector()){
        vs.add(vox);
      }
    }
    // Now, set the new voxel set in the output:
    ev_output.emplace(std::move(vs), std::move(meta));

  }

  return true;

}


void TensorFromCluster::finalize() {}
}

#include <pybind11/stl.h>


void init_tensor_from_cluster(pybind11::module m){


  using Class = larcv3::TensorFromCluster;
  pybind11::class_<Class> tensorfromcluster(m, "TensorFromCluster");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  tensorfromcluster.doc() = R"pbdoc(
      Convert SparseCluster objects (2D or 3D) into SparseTensor objects by merging
      all of the clusters together.  Voxels are added where necessary.
    )pbdoc";


  tensorfromcluster.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "TensorFromCluster");

  tensorfromcluster.def("default_config", &Class::default_config);
}
#endif
