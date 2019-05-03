/**
 * \file ProcessFactory.h
 *
 * \ingroup core_Processor
 * 
 * \brief Class def header for a class larcv3::ProcessFactory
 *
 * @author kazuhiro
 */

/** \addtogroup core_Processor

    @{*/
#ifndef __LARCV3PROCESSOR_PROCESSFACTORY_H
#define __LARCV3PROCESSOR_PROCESSFACTORY_H

#include <iostream>
#include <map>
#include "larcv3/core/base/larcv_base.h"
#include "larcv3/core/processor/ProcessBase.h"

#include <mutex>

#ifndef SWIG
static std::mutex __procfactory_mtx;
#endif

namespace larcv3 {

  /**
     \class ProcessFactoryBase
     \brief Abstract base class for factory (to be implemented per process)
  */
  class ProcessFactoryBase {
  public:
    /// Default ctor
    ProcessFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~ProcessFactoryBase(){}
    /// Abstract constructor method
    virtual ProcessBase* create(const std::string instance_name) = 0;
  };

  /**
     \class ProcessFactory
     \brief Factory class for instantiating process instance
     This factory class can instantiate a specified process instance w/ provided instance name. \n
     The actual factory core (to which each algorithm must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larcv3::ProcessFactory::get) \n
     to access a factory.
  */
  class ProcessFactory : public larcv_base  {

  public:
    /// Default ctor, shouldn't be used
    ProcessFactory() : larcv_base("ProcessFactory")
    {}
    /// Default dtor
    ~ProcessFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    inline static ProcessFactory& get()
    {
      __procfactory_mtx.lock();
      if(!_me) _me = new ProcessFactory; 
      __procfactory_mtx.unlock();
      return *_me;
    }
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(const std::string name, larcv3::ProcessFactoryBase* factory)
    { _factory_map[name] = factory; }
    /// Factory creation method (should be called by clients, possibly you!)
    ProcessBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LARCV_ERROR() << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      auto ptr = (*iter).second->create(instance_name);
      ptr->_typename = name;
      return ptr;
    }

  private:
    /// Static factory container
    std::map<std::string,larcv3::ProcessFactoryBase*> _factory_map;
    /// Static self
    static ProcessFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

