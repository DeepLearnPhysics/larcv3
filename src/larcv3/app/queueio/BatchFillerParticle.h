/**
 * \file BatchFillerParticle.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerParticle
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERPARTICLE_H__
#define __LARCV3THREADIO_BATCHFILLERPARTICLE_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"
#include "larcv3/core/dataformat/EventParticle.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerParticle ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerParticle : public BatchFillerTemplate<larcv3::ParticleHolder> {

  public:

    /// Default constructor
    BatchFillerParticle(const std::string name="BatchFillerParticle");

    /// Default destructor
    ~BatchFillerParticle(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void _batch_begin_();

    void _batch_end_();

    void finalize();

    static json default_config(){
      json c = {
        {"Producer", std::string()},
        {"MaxParticles", size_t(30)}
      };
      return c;
    }

  private:

    json config;

    std::vector<larcv3::ParticleHolder> _entry_data;
  };

  /**
     \class larcv3::BatchFillerParticleFactory
     \brief A concrete factory class for larcv3::BatchFillerParticle
  */
  class BatchFillerParticleProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerParticleProcessFactory() { ProcessFactory::get().add_factory("BatchFillerParticle",this); }
    /// dtor
    ~BatchFillerParticleProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerParticle(instance_name); }
  };

}

#ifdef LARCV_INTERNAL

void init_bf_particle(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group
