/**
 * \file ParentParticleSeg.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class ParentParticleSeg
 *
 * Merge all cluster2d voxel sets into a single set if they have a common ancestor
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __PARENTPARTICLESEG_H__
#define __PARENTPARTICLESEG_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
#include "larcv3/core/dataformat/Tensor.h"
#include "larcv3/core/dataformat/Particle.h"
#include "larcv3/core/dataformat/Voxel.h"

namespace larcv3 {

/**
   \class ProcessBase
   User defined class ParentParticleSeg ... these comments are
   used to generate
   doxygen documentation!
*/

struct particle_node{
  int trackID;
  int parentID;
  int ancestorID;
  bool primary;
  const Particle * reference;
  particle_node * parent;
  std::vector<const particle_node *> daughters;
};

class ParentParticleSeg : public ProcessBase {
 public:
  /// Default constructor
  ParentParticleSeg(
      const std::string name = "ParentParticleSeg");

  /// Default destructor
  ~ParentParticleSeg() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();

  larcv3::VoxelSet cluster_merger(const larcv3::SparseCluster2D & clusters,
                                 particle_node * primary_node);

  larcv3::VoxelSet cluster_merger(const larcv3::SparseCluster3D & clusters,
                                 particle_node * primary_node);
 private:

  void get_all_daughter_ids(std::vector<int> & ids, const particle_node * node);


  std::string _cluster3d_producer;
  std::string _cluster2d_producer;
  std::string _output_producer;
  std::string _particle_producer;

};

/**
   \class larcv3::ParentParticleSegFactory
   \brief A concrete factory class for larcv3::ParentParticleSeg
*/
class ParentParticleSegProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  ParentParticleSegProcessFactory() {
    ProcessFactory::get().add_factory("ParentParticleSeg",
                                      this);
  }
  /// dtor
  ~ParentParticleSegProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new ParentParticleSeg(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
