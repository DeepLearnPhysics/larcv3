/**
 * \file ParticleCountFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParticleCountFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV3FILTER_PARTICLECOUNTFILTER_H__
#define __LARCV3FILTER_PARTICLECOUNTFILTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class ParticleCountFilter ... these comments are used to generate
     doxygen documentation!
  */
  class ParticleCountFilter : public ProcessBase {

  public:
    
    /// Default constructor
    ParticleCountFilter(const std::string name="ParticleCountFilter");
    
    /// Default destructor
    ~ParticleCountFilter(){}

    void configure(const larcv3::PSet&);

    void initialize();

    bool process(larcv3::IOManager& mgr);

    void finalize();

  private:

    std::string _part_producer;
    size_t _max_part_count;
    size_t _min_part_count;
    std::vector<size_t> _part_count_v;
  };

  /**
     \class larcv3::ParticleCountFilterFactory
     \brief A concrete factory class for larcv3::ParticleCountFilter
  */
  class ParticleCountFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ParticleCountFilterProcessFactory() { ProcessFactory::get().add_factory("ParticleCountFilter",this); }
    /// dtor
    ~ParticleCountFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ParticleCountFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

