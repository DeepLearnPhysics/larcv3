#ifndef __GENFAKEPARTICLE_CXX__
#define __GENFAKEPARTICLE_CXX__

#include "GenFakeParticle.h"
#include "core/dataformat/EventParticle.h"

namespace larcv {

  static GenFakeParticleProcessFactory __global_GenFakeParticleProcessFactory__;

  GenFakeParticle::GenFakeParticle(const std::string name)
    : ProcessBase(name)
  {

  }

  void GenFakeParticle::configure(const PSet& cfg)
  {
    _particle_producer      = "test";
    num_particles_per_event = 5;
  }

  void GenFakeParticle::initialize()
  {}

  bool GenFakeParticle::process(IOManager& mgr)
  {
    auto& ev_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);


    if (ev_particle.size() != 0){
      LARCV_CRITICAL() << "Particles already present! Abort!" << std::endl;
    }


    return true;
  }

  void GenFakeParticle::finalize()
  {}

}
#endif
