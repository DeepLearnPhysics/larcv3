#ifndef __LARCV_BBOXFROMPARTICLE_CXX__
#define __LARCV_BBOXFROMPARTICLE_CXX__

#include "BBoxFromParticle.h"
#include "larcv3/core/dataformat/EventBBox.h"
#include "larcv3/core/dataformat/EventParticle.h"


namespace larcv3 {


static BBoxFromParticleProcessFactory
__global_BBoxFromParticleProcessFactory__;

BBoxFromParticle::BBoxFromParticle(const std::string name)
: ProcessBase(name) {}


void BBoxFromParticle::configure(const json& cfg) {

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

void BBoxFromParticle::initialize() {}

bool BBoxFromParticle::process(IOManager& mgr) {
  auto const& producer        = config["Producer"].get<std::string>();
  // auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  // if (product == "cluster2d")
  //   merge_clusters<EventSparseCluster2D, EventSparseTensor2D>(mgr, producer, output_producer);
  // else if (product == "cluster3d")
  //   merge_clusters<EventSparseCluster3D, EventSparseTensor3D>(mgr, producer, output_producer);
  // else{
  //   LARCV_CRITICAL() << "Encountered unsupported product for BBoxFromParticle: " << product << std::endl;
  //   throw larbys();
  // }

  bbox_from_particle<EventParticle, EventBBox3D>(mgr, producer, output_producer);

  return true;
}


template<class dataproduct_in, class dataproduct_out>
bool BBoxFromParticle::bbox_from_particle(IOManager& mgr, std::string producer, std::string output_producer){


  // Get the input and output objects:
  auto const ev_input  = mgr.template get_data<dataproduct_in>(producer);
  auto & ev_output     = mgr.template get_data<dataproduct_out>(output_producer);
  ev_output.clear();
  // We get the particles and create - from the vertex - a bbox in 3D of 0 size.

  BBoxCollection3D bbox_set;

  for (auto const & particle : ev_input.as_vector()){

    // What's the vertex of this particle?
    auto vertex = particle.position();

    // Create a new BBox3D
    BBox3D bb(
      {vertex.x(), vertex.y(), vertex.z()},
      {0.0, 0.0, 0.0});

    // Add to the collection:
    bbox_set.append(bb);

  }

  ev_output.emplace_back(std::move(bbox_set));


  return true;

}


void BBoxFromParticle::finalize() {}
}

#include <pybind11/stl.h>


void init_bbox_from_particle(pybind11::module m){


  using Class = larcv3::BBoxFromParticle;
  pybind11::class_<Class> BBoxFromParticle(m, "BBoxFromParticle");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  BBoxFromParticle.doc() = R"pbdoc(
      Create a bounding box from a particle by inspecting the particles vertex.
    )pbdoc";

  BBoxFromParticle.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "BBoxFromParticle");

  BBoxFromParticle.def("default_config", &Class::default_config);
}
#endif
