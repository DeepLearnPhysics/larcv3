/**
 * \file MCSingleParticleFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MCSingleParticleFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV3FILTER_MCSINGLEPARTICLEFILTER_H__
#define __LARCV3FILTER_MCSINGLEPARTICLEFILTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class MCSingleParticleFilter ... these comments are used to generate
     doxygen documentation!
  */
  class MCSingleParticleFilter : public ProcessBase {

  public:
    
    /// Default constructor
    MCSingleParticleFilter(const std::string name="MCSingleParticleFilter");
    
    /// Default destructor
    ~MCSingleParticleFilter(){}

    void configure(const larcv3::PSet&);

    void initialize();

    bool process(larcv3::IOManager& mgr);

    void finalize();

  private:

    std::string _part_producer;
    double _shower_min_energy;
    double _track_min_energy;
    double _proton_min_energy;
  };

  /**
     \class larcv3::MCSingleParticleFilterFactory
     \brief A concrete factory class for larcv3::MCSingleParticleFilter
  */
  class MCSingleParticleFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MCSingleParticleFilterProcessFactory() { ProcessFactory::get().add_factory("MCSingleParticleFilter",this); }
    /// dtor
    ~MCSingleParticleFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MCSingleParticleFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

