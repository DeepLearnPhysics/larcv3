/**
 * \file EventParticle.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventParticle
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV_EVENTPARTICLE_H
#define __LARCV_EVENTPARTICLE_H

#include "EventBase.h"
#include "Particle.h"
#include "DataProductFactory.h"



namespace larcv {
  /**
    \class EventParticle
    User-defined data product class (please comment!)
  */
  class EventParticle : public EventBase {
    
  public:
    
    /// Default constructor
    EventParticle(){}
    
    /// Default destructor
    ~EventParticle(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); _part_v.clear();}

    void set(const std::vector<larcv::Particle>& part_v);
    void append(const larcv::Particle& part);
    void emplace_back(larcv::Particle&& part);
    void emplace(std::vector<larcv::Particle>&& part_v);

    inline const std::vector<larcv::Particle>& as_vector() const
    { return _part_v; }
#ifndef SWIG
    void initialize(H5::Group * group);
    void serialize (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);
#endif

  private:

    std::vector<larcv::Particle> _part_v; ///< a collection of particles (index maintained)

  };
}


#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventParticle>() { return "particle"; }
  template<> EventParticle& IOManager::get_data<larcv::EventParticle>(const std::string&);
  template<> EventParticle& IOManager::get_data<larcv::EventParticle>(const ProducerID_t);

  /**
     \class larcv::EventParticle
     \brief A concrete factory class for larcv::EventParticle
  */
  class EventParticleFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventParticleFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventParticle>(),this); }
    /// dtor
    ~EventParticleFactory() {}
    /// create method
    EventBase* create() { return new EventParticle; }
  };

  
}


#endif
/** @} */ // end of doxygen group 

