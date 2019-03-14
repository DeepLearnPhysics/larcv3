/**
 * \file GenFakeParticle.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class GenFakeParticle
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __GENFAKEPARTICLE_H__
#define __GENFAKEPARTICLE_H__

#include "larcv/core/processor/ProcessBase.h"
#include "larcv/core/processor/ProcessFactory.h"

#include <vector>
#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class GenFakeParticle ... these comments are used to generate
     doxygen documentation!
  */
  class GenFakeParticle : public ProcessBase {

  public:
    
    /// Default constructor
    GenFakeParticle(const std::string name="GenFakeParticle");
    
    /// Default destructor
    ~GenFakeParticle(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _particle_producer;
    size_t num_particles_per_event;
  };

  /**
     \class larcv::GenFakeParticleFactory
     \brief A concrete factory class for larcv::GenFakeParticle
  */
  class GenFakeParticleProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    GenFakeParticleProcessFactory() { ProcessFactory::get().add_factory("GenFakeParticle",this); }
    /// dtor
    ~GenFakeParticleProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new GenFakeParticle(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

