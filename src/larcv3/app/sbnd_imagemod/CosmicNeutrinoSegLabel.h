/**
 * \file CosmicNeutrinoSegLabel.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class CosmicNeutrinoSegLabel
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV_COSMICNEUTRINOSEGLABEL_H__
#define __LARCV_COSMICNEUTRINOSEGLABEL_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
#include "larcv3/core/dataformat/Tensor.h"
#include "larcv3/core/dataformat/Particle.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

namespace larcv3 {

/**
   \class ProcessBase
   User defined class CosmicNeutrinoSegLabel ... these comments are
   used to generate
   doxygen documentation!
*/
class CosmicNeutrinoSegLabel : public ProcessBase {
 public:

  enum particleLabel {kBackground, kCosmic, kNeutrino};

  /// Default constructor
  CosmicNeutrinoSegLabel(
      const std::string name = "CosmicNeutrinoSegLabel");

  /// Default destructor
  ~CosmicNeutrinoSegLabel() {}

  void configure(const json&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();

  template <size_t dimension>
  SparseTensor<dimension> seg_image_creator(const std::vector<Particle> & particles,
                            const SparseCluster<dimension> & clusters,
                            const ImageMeta<dimension> & meta,
                            const int neutrino_label,
                            const int cosmic_label);
  static json default_config(){
      json c = {
        {"Cluster2dProducer", ""},
        {"Cluster3dProducer", ""},
        {"OutputProducer",    ""},
        {"ParticleProducer",  ""},
        {"NeutrinoLabel", 1},
        {"CosmicLabel", 2}
      };
      return c;
  }

 private:

  json config;


};

/**
   \class larcv3::CosmicNeutrinoSegLabelFactory
   \brief A concrete factory class for larcv3::CosmicNeutrinoSegLabel
*/
class CosmicNeutrinoSegLabelProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  CosmicNeutrinoSegLabelProcessFactory() {
    ProcessFactory::get().add_factory("CosmicNeutrinoSegLabel",
                                      this);
  }
  /// dtor
  ~CosmicNeutrinoSegLabelProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new CosmicNeutrinoSegLabel(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
