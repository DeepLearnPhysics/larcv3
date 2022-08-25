#ifndef __LARCV_COSMICNEUTRINOSEGLABEL_CXX__
#define __LARCV_COSMICNEUTRINOSEGLABEL_CXX__

#include "CosmicNeutrinoSegLabel.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventParticle.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"

namespace larcv3 {

static CosmicNeutrinoSegLabelProcessFactory
    __global_CosmicNeutrinoSegLabelProcessFactory__;

CosmicNeutrinoSegLabel::CosmicNeutrinoSegLabel(
    const std::string name)
    : ProcessBase(name) {}

void CosmicNeutrinoSegLabel::configure(const json& cfg) {
  // Grab the default config:
  config = this->default_config();
  config = augment_default_config(config, cfg);

  const auto & cluster2dproducer = config["Cluster2dProducer"].get<std::string>();
  const auto & output_producer   = config["OutputProducer"].get<std::string>();
  const auto & particle_producer = config["ParticleProducer"].get<std::string>();

  if (cluster2dproducer.empty()){
        LARCV_CRITICAL() << "Must specify cluster producer!" << std::endl;
        throw larbys();
  }
  if (output_producer.empty()) {
    LARCV_CRITICAL() << "Must specify an OutputProducer" << std::endl;
    throw larbys();
  }
  if (particle_producer.empty()) {
    LARCV_CRITICAL() << "Must specify an ParticleProducer" << std::endl;
    throw larbys();
  }
}

void CosmicNeutrinoSegLabel::initialize() {}

bool CosmicNeutrinoSegLabel::process(IOManager& mgr) {

  auto _cluster2d_producer = config["Cluster2dProducer"].get<std::string>();
  auto _cluster3d_producer = config["Cluster3dProducer"].get<std::string>();
  auto _output_producer    = config["OutputProducer"].get<std::string>();
  auto _particle_producer  = config["ParticleProducer"].get<std::string>();
  auto cosmic_label        = config["CosmicLabel"].get<int>();
  auto neutrino_label      = config["NeutrinoLabel"].get<int>();


  // Read in the particles that define the pdg types:
  auto const& ev_particle =
      mgr.get_data<larcv3::EventParticle>(_particle_producer);

  if (_cluster2d_producer != ""){
    // Read in the original source of segmentation, the cluster indexes:
    auto const& ev_cluster2d =
        mgr.get_data<larcv3::EventSparseCluster2D>(_cluster2d_producer);

    // The output is an instance of image2D, so prepare that:
    auto& ev_tensor2d_output = mgr.get_data<larcv3::EventSparseTensor2D>(_output_producer);
    ev_tensor2d_output.clear();
    // Next, loop over the particles and clusters per projection_ID
    // and set the values in the output image to the label specified by
    // the pdg

    auto const& particles = ev_particle.as_vector();
    for (size_t projection_index = 0;
         projection_index < ev_cluster2d.size(); ++projection_index) {
      // For each projection index, get the list of clusters
      auto const& clusters = ev_cluster2d.sparse_cluster(projection_index);

      auto const& out_image =
          seg_image_creator<2>(particles, clusters,
                            ev_cluster2d.sparse_cluster(projection_index).meta(),
                            neutrino_label, cosmic_label);

      // Append the output image2d:
      ev_tensor2d_output.set(out_image);
    }
  }


  if (_cluster3d_producer != ""){
    // Read in the original source of segmentation, the cluster indexes:
    auto const& ev_cluster3d =
      mgr.get_data<larcv3::EventSparseCluster3D>(_cluster3d_producer);

    // The output is an instance of image2D, so prepare that:
    auto& ev_tensor3d_output = mgr.get_data<larcv3::EventSparseTensor3D>(_output_producer);
    ev_tensor3d_output.clear();
    // Next, loop over the particles and clusters per projection_ID
    // and set the values in the output image to the label specified by
    // the pdg

    auto const& particles = ev_particle.as_vector();
    for (size_t projection_index = 0;
         projection_index < ev_cluster3d.size(); ++projection_index) {
      // For each projection index, get the list of clusters
      auto const& clusters = ev_cluster3d.sparse_cluster(projection_index);

      auto const& out_image =
          seg_image_creator<3>(particles, clusters,
                            ev_cluster3d.sparse_cluster(projection_index).meta(),
                            neutrino_label, cosmic_label);

      // Append the output image3d:
      ev_tensor3d_output.set(out_image);
    }
  }

  return true;
}

template <size_t dimension>
SparseTensor<dimension> CosmicNeutrinoSegLabel::seg_image_creator(
    const std::vector<Particle> & particles,
    const SparseCluster<dimension> & clusters, 
    const ImageMeta<dimension> & meta,
    const int neutrino_label, const int cosmic_label) {
  // Prepare an output SparseTensor<dimension>:
  SparseTensor<dimension> out_image;
  // Set the meta:
  out_image.meta(meta);

  // std::set<std::string> processes;
  // std::set<int> interaction_types;

  Voxel invalid_voxel = kINVALID_VOXEL::getInstance();

  // Loop over the particles and get the cluster that matches:
  for (size_t particle_index = 0; particle_index < particles.size();
       ++particle_index) {
    // Deterime this particles PDG code and if it's in the list:
    auto const& particle = particles.at(particle_index);
    particleLabel pixel_label = kBackground;
    // processes.insert(particle.creation_process());
    // interaction_types.insert(particle.nu_interaction_type());
    if (particle.nu_interaction_type() == neutrino_label){
      pixel_label = kNeutrino;
      // std::cout << "Found a neutrino" << std::endl;
    }
    else if (particle.nu_interaction_type() == cosmic_label) {
      pixel_label = kCosmic;
    }
    // If the label is not zero, go ahead and set the pixels to this label
    // in the output image:
    if (pixel_label != 0) {
      auto const& cluster = clusters.as_vector().at(particle_index);
      // Loop over the pixels in this cluster:
      for ( size_t voxel_index = 0;  
            voxel_index < cluster.as_vector().size();
            ++voxel_index) {
        auto const& voxel = cluster.as_vector().at(voxel_index);

        // if (out_image.pixel(voxel.id()) != pixel_label){
        //   std::cout << "Changing pixel value from " << out_image.pixel(voxel.id())
        //             << " to " << pixel_label << std::endl;
        // }

        // We have to make sure that we don't overwrite a neutrino pixel with a cosmic pixel.
        // We do that by writing in two cases:
        // 1) There is no pixel currently set at that ID
        // 2) The current label is neutrino, so write no matter what.
        
        auto current_voxel = out_image.find(voxel.id());
        if (current_voxel == invalid_voxel || pixel_label == kNeutrino){
          // Then, this voxel wasn't found, add it:
          out_image.emplace(Voxel(voxel.id(), pixel_label), false);
        } 
      }
    }
  }
  // for (auto & process : processes){
  //   std::cout << "Process: " << process << std::endl;
  // }
  // for (auto & type : interaction_types){
  //   std::cout << "type: " << type << std::endl;
  // }


  return out_image;
}

void CosmicNeutrinoSegLabel::finalize() {}
}
#endif
