#ifndef __LARCV_BBOXFROMCLUSTER_CXX__
#define __LARCV_BBOXFROMCLUSTER_CXX__

#include "BBoxFromCluster.h"
#include "larcv3/core/dataformat/EventBBox.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"


namespace larcv3 {


static BBoxFromClusterProcessFactory
__global_BBoxFromClusterProcessFactory__;

BBoxFromCluster::BBoxFromCluster(const std::string name)
: ProcessBase(name) {}


void BBoxFromCluster::configure(const json& cfg) {

  config = this -> default_config();
  config = augment_default_config(config, cfg);

  // We're really just verifying that everything is a filled
  auto const& producer        = config["Producer"].get<std::string>();
  // auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  if (producer.empty()) {
    LARCV_CRITICAL() << "Must specify a producer" << std::endl;
    throw larbys();
  }
  // if (product.empty()) {
  //   LARCV_CRITICAL() << "Must specify a Product" << std::endl;
  //   throw larbys();
  // }
  if (output_producer.empty()) {
    LARCV_CRITICAL() << "Must specify an OutputProducer" << std::endl;
    throw larbys();
  }
}

void BBoxFromCluster::initialize() {}

bool BBoxFromCluster::process(IOManager& mgr) {
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  if (product == "cluster2d")
    bbox_from_cluster<2>(mgr, producer, output_producer);
  else if (product == "cluster3d")
    bbox_from_cluster<3>(mgr, producer, output_producer);
  else{
    LARCV_CRITICAL() << "Encountered unsupported product for BBoxFromCluster: " << product << std::endl;
    throw larbys();
  }

  return true;
}


template<size_t dimension>
bool BBoxFromCluster::bbox_from_cluster(IOManager& mgr, std::string producer, std::string output_producer){

  float threshold = config["Threshold"].get<float>(); // defaults to 0.0
  int min_voxels  = config["MinVoxels"].get<int>(); // defaults to 1


  // Get the input and output objects:
  auto const ev_input  = mgr.template get_data<EventSparseCluster<dimension>>(producer);
  auto & ev_output     = mgr.template get_data<EventBBox<dimension>>(output_producer);
  ev_output.clear();
  // We get the clusters and create - from the voxels - a bbox.
  // Loop over projection index:
  for (size_t projection_id = 0; projection_id < ev_input.size(); projection_id ++){

    // Get the sparse cluster:
    auto sparse_cluster = ev_input.at(projection_id);
    auto meta = sparse_cluster.meta();

    // Create a collection of output bboxes
    BBoxCollection<dimension> bbox_set(meta);


    for (auto const & cluster : sparse_cluster.as_vector()){

      // we need to find the min and max of the voxels.
      // Save it into an array:
      std::array<double, dimension> _min;
      std::array<double, dimension> _max;
      // check if there are any voxels:
      if (cluster.size() == 0){
        for (size_t i_dim = 0; i_dim < dimension; i_dim ++){
          _min[i_dim] = 0.0;
          _max[i_dim] = 0.0;
        }
      }
      else{
        // initialize the min max to ridiculous values:
        for (size_t i_dim = 0; i_dim < dimension; i_dim ++){
          _min[i_dim] =  9999.;
          _max[i_dim] = -9999.;
        }
        // loop over the voxels to find the min and max
        bool found_over_threshold = false;
        int n_found = 0;
        for (auto const & voxel : cluster.as_vector()){
          if (voxel.value() > threshold){
            found_over_threshold = true;
            n_found ++;
            // Figure out the nD coordinates of this voxel:
            std::vector<double> position = meta.position(voxel.id());
            for (size_t i_dim = 0; i_dim < dimension; i_dim ++){
              // Compare each dimension:
              if (position[i_dim] < _min[i_dim]){
                // std::cout << "Lesser position: (" << position[0] << ", " << position[1] << ") vs ("
                          // << _min[0] << ", "<< _min[1] << ")";
                _min[i_dim] = position[i_dim];
                // std::cout << " Updated to (" << _min[0] << ", "<< _min[1] << ")" << std::endl;
              }
              if (position[i_dim] > _max[i_dim]){
                // std::cout << "greater position: (" << position[0] << ", " << position[1] << ") vs ("
                          // << _max[0] << ", ("<< _max[1] << ")";
                _max[i_dim] = position[i_dim];
                // std::cout << " Updated to (" << _max[0] << ", "<< _max[1] << ")" << std::endl;
              }
            }

          }
        }
        // Final check: if no voxels were above threshold, revert to 0:
        if (! found_over_threshold || n_found < min_voxels){
          for (size_t i_dim = 0; i_dim < dimension; i_dim ++){
            _min[i_dim] = 0.0;
            _max[i_dim] = 0.0;
          }
        }
        // else{
        //   std::cout << "Final min/max (0): " << _min[0] << ", " << _max[0] << std::endl;
        //   std::cout << "Final min/max (1): " << _min[1] << ", " << _max[1] << std::endl;
        // }
      }

      std::array<double, dimension> _centroid;
      std::array<double, dimension> _half_length;
      // Convert the min and max to a centroid and half length:
      for (size_t i_dim = 0; i_dim < dimension; i_dim ++){
        _centroid[i_dim]    = 0.5*(_min[i_dim] + _max[i_dim]);
        _half_length[i_dim] = 0.5*(_max[i_dim] - _min[i_dim]);
      }

      // What's the vertex of this particle?

      // Create a new BBox3D
      BBox<dimension> bb(_centroid, _half_length);

      // Add to the collection:
      bbox_set.append(bb);
      // std::cout << "BBox set size: " << bbox_set.size() << "(" << cluster.size() <<" voxels)" << std::endl;
    }

    ev_output.emplace_back(std::move(bbox_set));
  }


  return true;

}


void BBoxFromCluster::finalize() {}
}

#include <pybind11/stl.h>


void init_bbox_from_cluster(pybind11::module m){


  using Class = larcv3::BBoxFromCluster;
  pybind11::class_<Class> BBoxFromCluster(m, "BBoxFromCluster");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  BBoxFromCluster.doc() = R"pbdoc(
      Create a set of Bounding Boxes from a set of Sparse Clusters by enclosing each
      cluster in a minimally sized box along each axis.

      In other words, draw a box around each cluster as small as possible.
    )pbdoc";

  BBoxFromCluster.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "BBoxFromCluster");

  BBoxFromCluster.def("default_config", &Class::default_config);
}
#endif
