#ifndef __PARENTPARTICLESEG_CXX__
#define __PARENTPARTICLESEG_CXX__

#include <iomanip>

#include "ParentParticleSeg.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventParticle.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

static ParentParticleSegProcessFactory
    __global_ParentParticleSegProcessFactory__;

ParentParticleSeg::ParentParticleSeg(const std::string name)
    : ProcessBase(name) {}

void ParentParticleSeg::configure(const json& cfg) {

  // Grab the default config:
  config = this->default_config();
  config = augment_default_config(config, cfg);

  const auto & cluster2dproducer = config["Cluster2dProducer"].get<std::string>();
  const auto & cluster3dproducer = config["Cluster3dProducer"].get<std::string>();
  const auto & output_producer   = config["OutputProducer"].get<std::string>();
  const auto & particle_producer = config["ParticleProducer"].get<std::string>();

  if (cluster2dproducer.empty() && cluster3dproducer.empty()){
        LARCV_CRITICAL() << "Must specify at least one cluster producer!" << std::endl;
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

void ParentParticleSeg::initialize() {}

bool ParentParticleSeg::process(IOManager& mgr) {

  auto const& cluster2d_producer = config["Cluster2dProducer"].get<std::string>();
  auto const& cluster3d_producer = config["Cluster3dProducer"].get<std::string>();
  auto const& output_producer    = config["OutputProducer"].get<std::string>();
  auto const& particle_producer  = config["ParticleProducer"].get<std::string>();

  // std::cout << "Enter ParentParticleSeg::process " << std::endl;
  // Read in the particles that define the pdg types:
  auto const& ev_particle =
      mgr.get_data<larcv3::EventParticle>(particle_producer);


  bool debug = false;

  if (debug){
    std::cout << "Initial number of particles: " << ev_particle.as_vector().size() << std::endl;
  }

  // Loop over all of the input particles and determine their ancestor


  // Make a particle node for every particle (to build a particle tree)
  std::vector<particle_node*> particle_nodes;

  // Additionally, keep a short list of just the primaries (pointers)
  std::vector<particle_node*> primary_nodes;

  particle_node * orphanage = new particle_node();


  std::set<int> _ancestor_trackids;
  std::set<int> _found_ancestor_nodes;

  for (auto& particle : ev_particle.as_vector()) {
    if (debug){
      // std::cout << "Particle: "
      //           << "ancestor track ID " << particle.ancestor_track_id() << ", "
      //           << "ancestor PDG " << particle.ancestor_pdg_code() << ", "
      //           << "ID " << particle.id() << ", "
      //           << "parent ID " << particle.parent_track_id() << ", "
      //           << "PDG " << particle.pdg_code() << ", "
      //           << "track ID " << particle.track_id() << "." << std::endl;
    }

    // Particles are top level if their ancestor ID == their own track ID

    // Create a particle_node object:
    particle_node * node = new particle_node();
    particle_nodes.push_back(node);
    node->reference = &particle;
    node->trackID = particle.track_id();
    node->parentID = particle.parent_track_id();
    node->ancestorID = particle.ancestor_track_id();
    node->is_virtual = false;

    // Primary?
    // if (particle.ancestor_track_id() == particle.track_id() ||
    //     particle.ancestor_pdg_code() == 111 ||  // neutral particles make new primaries
    //     particle.ancestor_pdg_code() == 2112)
    if (particle.creation_process() == "primary")
    {
      // Yes, it's a primary
      if (debug) {
        std::cout << "Primary Particle: "
                << "ancestor track ID " << particle.ancestor_track_id() << ", "
                << "ancestor PDG " << particle.ancestor_pdg_code() << ", "
                << "ID " << particle.id() << ", "
                << "parent ID " << particle.parent_track_id() << ", "
                << "PDG " << particle.pdg_code() << ", "
                << "track ID " << particle.track_id() << "." << std::endl;
        // std::cout << "-> Primary "
        //           << particle.track_id() << " - address: " << node << std::endl;
      }
      node->primary = true;
      node->parent = NULL;
      primary_nodes.push_back(node);
      _found_ancestor_nodes.insert(particle.ancestor_track_id());
    }
    else{
      // No, not a primary
      node->primary = false;
    }
    //
    // Keep track of all ancestor ids found:
    _ancestor_trackids.insert(particle.ancestor_track_id());

  }

  // Store a list of pointers to update later when daughters are sorted out.
  std::vector<particle_node* > virtual_ancestors;
  std::vector<Particle> new_ancestors;
  // Save enough room for everything:
  new_ancestors.reserve(_ancestor_trackids.size());
  size_t i_virtual = 0;
  // Fill out the rest of the ancestor nodes:
  for (auto& trackID : _ancestor_trackids) {
    if (_found_ancestor_nodes.find(trackID) == _found_ancestor_nodes.end()) {
      // This ancestor id isn't a tangible particle


      // Make a new particle
      new_ancestors.push_back(Particle());
      new_ancestors[i_virtual].track_id(trackID);


      particle_node * pn = new particle_node;
      pn->trackID = trackID;
      pn->primary = trackID;
      pn->is_virtual = true;
      pn->reference = &(new_ancestors[i_virtual]);
      pn->parent = NULL;

      virtual_ancestors.push_back(pn);


      // The reference particle will be the end one:
      virtual_ancestors[i_virtual]->reference = & (new_ancestors[i_virtual]);
      virtual_ancestors[i_virtual]->parent = NULL;
      virtual_ancestors[i_virtual]->primary = true;
      virtual_ancestors[i_virtual]->trackID = trackID;
      virtual_ancestors[i_virtual]->is_virtual = true;

      // Assign the addresses to the list of pointers.
      particle_nodes.push_back(virtual_ancestors[i_virtual]);
      primary_nodes.push_back(virtual_ancestors[i_virtual]);


      // For Particles that don't *actually* exist, we care about the interaction
      // type for cosmic tagger.  Daughter particles have the same interaction type
      // so we pick that up below.
      // std::cout << "Just created virtual primary particle " << trackID
      //           << ", reference: " << virtual_ancestors[i_virtual]->reference
      //           << ", number of daughters: " << virtual_ancestors[i_virtual]->daughters.size()
      //           << std::endl;
      i_virtual ++;
    }
    // std::cout << "Ancestor ID " << trackID << std::endl;
  }

  if (debug){

    std::cout << "virtual_ancestors: \n";
    for (auto va : virtual_ancestors){
      std::cout << "  trackID: " << va->trackID << ", is_virtual: " << va->is_virtual
                << ", reference: " << va->reference << "\n";
    }
    std::cout << std::endl;

    for (auto node : primary_nodes){
      std::cout << "Found the following primary: " << node->trackID  << " at " << node->reference << std::endl;
    }
  }

  if (debug){
    std::cout << "Created " << particle_nodes.size() << " total nodes." << std::endl;
  }

  // Now, every particle has a corresponding particle node,
  // And there are particle nodes made for the trackIDs that aren't actually
  // particles (Didn't ionize, probably)

  // Loop through the list of particle nodes, and find the parent node
  // of each particle.  To speed up a little, since most particles
  // are only one layer deep, check the ancestor nodes first
  for (auto node : particle_nodes){
    // Skip primary nodes
    if (node->primary) continue;

    bool found = false;

    // This code sorts into a full tree:
    for (auto primary_node : primary_nodes){
      if (node->parentID == primary_node -> trackID){
        primary_node -> daughters.push_back(node);
        // std::cout << "Found node " << primary_node->trackID << " as parent of " << node->trackID << std::endl;
        found = true;
        break;
      }
    }
    if (!found){
      for (auto & node2 : particle_nodes){
        if (node->parentID == node2->trackID){
          node2->daughters.push_back(node);
          found = true;
          break;
        }
      }
    }
    if (! found){
      // Try to match this particle's ancestor with a primary node
      found = false;
      for (auto primary_node : primary_nodes){
        if (node->ancestorID == primary_node -> trackID){
          primary_node -> daughters.push_back(node);
          // std::cout << "Found node " << primary_node->trackID << " as parent of " << node->trackID << std::endl;
          found = true;
          break;
        }
      }
      // std::cout << "node with track ID " << node->trackID << " is orphaned." << std::endl;
      if (!found)
        orphanage -> daughters.push_back(node);
    }

  }
  // Here, all the virtual ancestors are updated with an interaction type
  // from their daughters
  for (size_t i_va = 0; i_va < virtual_ancestors.size(); i_va ++){
    // Identify the interaction type from the daughters
    if (virtual_ancestors.at(i_va)->daughters.size() > 0){
      auto interaction_type = virtual_ancestors.at(i_va)->daughters.front()->reference->nu_interaction_type();
      // auto replacement = new Particle(*virtual_ancestor.reference);
      // replacement->nu_interaction_type(interaction_type);
      // virtual_ancestor.reference->nu_interaction_type(interaction_type);
      new_ancestors.at(i_va).nu_interaction_type(interaction_type);
    }
  }




  if (debug){
    for (auto ancestor_node : primary_nodes){
      std::cout << "Top level particle.  TrackID is "
                << ancestor_node->trackID
                << ", id: " << ancestor_node->reference->id()
                << ", number of daughers: "
                << ancestor_node->daughters.size()
                << ", is_virtual: " << ancestor_node->is_virtual
                << ", reference: " << ancestor_node->reference
                // << ", interaction_type: " << ancestor_node->reference->nu_interaction_type()
                << std::endl;
      // for (auto daughter : ancestor_node->daughters){
      //   std::cout << "--> daughter trackID " << daughter->trackID << std::endl;
      //   // std::cout << "--> daughter trackID " << daughter->trackID << ", id " << daughter->reference->id() << std::endl;
      // }
    }
    // Print out the orphaned particles too:
    std::cout << "Orphanage: " <<std::endl;
    for (auto & particle : orphanage->daughters){
        std::cout << "--> orphan trackID " << particle->trackID << std::endl;
    }
  }
  // Here, every particle is sorted into it's own group by ancestor.


  // Now, make a corresponding particle 2d to match the clusters.
  auto& ev_particle_output =
      mgr.get_data<larcv3::EventParticle>(output_producer);
    // std::cout << "Initial number of output particles: " << ev_particle_output.as_vector().size() << std::endl;

  ev_particle_output.clear();
    // std::cout << "Initial number of output particles: " << ev_particle_output.as_vector().size() << std::endl;

  // Make the appropriate list of new particles:
  for (auto node : primary_nodes){
    if (node -> reference != NULL){
      // if (debug) std::cout << "Appending particle " << std::endl;
      ev_particle_output.append(*(node->reference));
    }
    else{
      ev_particle_output.append(Particle());
    }
  }

  if (debug) std::cout << "New particles created " << std::endl;

  // We now loop over the clusters indicated and merge them together based on

  if (cluster2d_producer != ""){
    // Read in the original source of segmentation, the cluster indexes:
    auto const& ev_cluster2d =
        mgr.get_data<larcv3::EventSparseCluster2D>(cluster2d_producer);

    if (ev_cluster2d.as_vector().size() < 2) return false;
    // The output is an instance of cluster2d, so prepare that:
    auto& ev_cluster2d_output =
        mgr.get_data<larcv3::EventSparseCluster2D>(output_producer);


    for (size_t projection_index = 0;
         projection_index < ev_cluster2d.as_vector().size(); ++projection_index) {
      // For each projection index, get the list of clusters
      auto const& clusters = ev_cluster2d.sparse_cluster(projection_index);
      if (clusters.size() == 1) return false;
      larcv3::SparseCluster2D new_clusters;
      new_clusters.meta(clusters.meta());

      // if (debug) std::cout << "Meta created for projection_index " << projection_index << std::endl;
      int i = 0;
      for (auto ancestor_node : primary_nodes) {
        auto out_cluster = cluster_merger(clusters, ancestor_node);
        // if (debug) std::cout << "Cluster merged with " << out_cluster.size() << " voxels.";
        out_cluster.id(i);
        i++;
        new_clusters.emplace(std::move(out_cluster));
        // std::cout << "done" << std::endl;
      }
      // if (debug) std::cout << "All owned particles merged" << std::endl;
      // Handle the orphanage, as well:
      orphanage->trackID = i;
      auto out_cluster = cluster_merger(clusters, orphanage);
      new_clusters.emplace(std::move(out_cluster));
      // if (debug) {
      //   std::cout << "Number of primary_nodes: " << primary_nodes.size()
      //            << std::endl;
      //   std::cout << "Number of new clusters: " << new_clusters.size() << std::endl;
      // }
      // Append the output image2d:
      ev_cluster2d_output.emplace(std::move(new_clusters));
    }
    // if (debug) std::cout << "Final number of clusters2d: " << ev_cluster2d_output.sparse_cluster(0).as_vector().size() << std::endl;
  }

  if (cluster3d_producer != ""){

    // Read in the original source of segmentation, the cluster indexes:
    auto const& ev_cluster3d =
        mgr.get_data<larcv3::EventSparseCluster3D>(cluster3d_producer);

    // if (debug) {
    //   std::cout << "ev_cluster3d.size() " << ev_cluster3d.size() << std::endl;
    // }
    // // if (ev_cluster3d.size() < 2) return false;

    // The output is an instance of cluster3d, so prepare that:
    auto& ev_cluster3d_output =
        mgr.get_data<larcv3::EventSparseCluster3D>(output_producer);


    // For each projection index, get the list of clusters
    larcv3::SparseCluster3D new_clusters;
    new_clusters.meta(ev_cluster3d.as_vector().front().meta());

    if(ev_cluster3d.sparse_cluster(0).as_vector().size() == 1) return false;

    int i = 0;
    for (auto ancestor_node : primary_nodes) {
      auto out_cluster = cluster_merger(ev_cluster3d.sparse_cluster(0), ancestor_node);
      out_cluster.id(i);
      i++;
      new_clusters.insert(out_cluster);
    }
    // Handle the orphanage, as well:
    orphanage->trackID = i;
    auto out_cluster = cluster_merger(ev_cluster3d.sparse_cluster(0), orphanage);
    new_clusters.insert(out_cluster);

    // Append the output image2d:
    ev_cluster3d_output.emplace(std::move(new_clusters));

  }



  // Final numbers:
  // std::cout << "Final number of particles: " << ev_particle_output.as_vector().size() << std::endl;

  // Clean up:
  for (auto node : particle_nodes){
    // if (debug) std::cout << "Delete node at " << node << "?: ";
    // Don't delete the virtual anscestors, they will clean up automatically.
    delete node;
    // if (! node->is_virtual){
    //   // if (debug) std::cout << "yes." << std::endl;
    //   delete node;
    // }
    // else{
    //   // if (debug) std::cout << "no." << std::endl;

    // }

  }
  delete orphanage;

  // std::cout << "Exit ParentParticleSeg::process " << std::endl;


  return true;
}

void ParentParticleSeg::get_all_daughter_ids(std::vector<size_t> & ids, const particle_node * node){
  if (node -> reference != NULL){
    // std::cout << "reference is " << node->reference << std::endl;
    ids.push_back(node->reference->id());
  }
#ifdef LARCV_OPENMP
#pragma omp parallel
#endif
  for (auto const daughter : node -> daughters){
    get_all_daughter_ids(ids, daughter);
  }
  return;
}

larcv3::VoxelSet ParentParticleSeg::cluster_merger(
    const larcv3::SparseCluster2D& clusters, particle_node * primary_node) {
  // Create an empty voxelset to hold the output:
  larcv3::VoxelSet output_set;
  output_set.id(primary_node->trackID);

  // if (primary_node -> reference == NULL){
  //   return output_set;
  // }

  std::vector<size_t> cluster_indexes;

  get_all_daughter_ids(cluster_indexes, primary_node);

  // std::cout << "For particle with track ID " << primary_node->trackID << ", "
  //           << "the following cluster indexes are to be merged: " << std::endl;
  // for (auto & index : cluster_indexes) std::cout << " - " << index << std::endl;


  for (auto id : cluster_indexes) {
    // Make sure the target cluster exists :
    if (id > clusters.size()) continue;
    auto& input_cluster = clusters.voxel_set(id);
#ifdef LARCV_OPENMP
#pragma omp parallel
#endif
    for (auto& voxel : input_cluster.as_vector()) {
      output_set.add(larcv3::Voxel(voxel.id(), voxel.value()));
    }
  }


  return output_set;
}

larcv3::VoxelSet ParentParticleSeg::cluster_merger(
    const larcv3::SparseCluster3D & clusters, particle_node * primary_node) {
  // Create an empty voxelset to hold the output:
  larcv3::VoxelSet output_set;
  output_set.id(primary_node->trackID);

  if (primary_node -> reference == NULL){
    return output_set;
  }

  std::vector<size_t> cluster_indexes;

  get_all_daughter_ids(cluster_indexes, primary_node);

  // std::cout << "Clusters.size() " << clusters.as_vector().size() << std::endl;
  for (auto id : cluster_indexes) {
    if (id > clusters.size()) continue;
    auto& input_cluster = clusters.voxel_set(id);
    // std::cout << "Cluster index " << id << ", number of voxels: " << input_cluster.as_vector().size() << std::endl;
#ifdef LARCV_OPENMP
#pragma omp parallel
#endif
    for (auto& voxel : input_cluster.as_vector()) {
      if (voxel.id() >= clusters.meta().total_voxels()){
        continue;
      }
      output_set.add(larcv3::Voxel(voxel.id(), voxel.value()));
    }
  }


  return output_set;
}


void ParentParticleSeg::finalize() {}

} // namespace larcv3

void init_parent_particle_seg(pybind11::module m){


  using Class = larcv3::ParentParticleSeg;
  pybind11::class_<Class> parent_particle_seg(m, "ParentParticleSeg");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  parent_particle_seg.doc() = R"pbdoc(
      Reconstructs the particle hierarchy, and then merges the particles into their highest parent.

      Assumes a 1 to 1 match of larcv::Particle to clusters in 2D and 3D (across all projection ids)
      and also merges the clusters into their highest level parent.

      Useful to merge the segmentation labels of an initial state particle to contain all of its children.
    )pbdoc";


  parent_particle_seg.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "ParentParticleSeg");

  parent_particle_seg.def("default_config", &Class::default_config);
}


#endif
