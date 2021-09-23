/**
 * \file BBoxFromParticle.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class BBoxFromParticle
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV_BBOXFROMPARTICLE_H__
#define __LARCV_BBOXFROMPARTICLE_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BBoxFromParticle ... these comments are used to generate
     doxygen documentation!
  */
  class BBoxFromParticle : public ProcessBase {

  public:

    /// Default constructor
    BBoxFromParticle(const std::string name="BBoxFromParticle");

    /// Default destructor
    ~BBoxFromParticle(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"OutputProducer", std::string()},
        };
        return c;
    }

  private:

    json config;

    template<class dataproduct_in, class dataproduct_out>
    bool bbox_from_particle(IOManager& mgr, std::string producer, std::string output_producer);

  };

  /**
     \class larcv3::BBoxFromParticleProcessFactory
     \brief A concrete factory class for larcv3::BBoxFromParticle
  */


  class BBoxFromParticleProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BBoxFromParticleProcessFactory() { ProcessFactory::get().add_factory("BBoxFromParticle",this); }
    /// dtor
    ~BBoxFromParticleProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BBoxFromParticle(instance_name); }
  };


}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_bbox_from_particle(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

