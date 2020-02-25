/**
 * \file EventIDFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EventIDFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV3FILTER_EVENTIDFILTER_H__
#define __LARCV3FILTER_EVENTIDFILTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
#include <map>
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class EventIDFilter ... these comments are used to generate
     doxygen documentation!
  */
  class EventIDFilter : public larcv3::ProcessBase {

  public:
    
    /// Default constructor
    EventIDFilter(const std::string name="EventIDFilter");
    
    /// Default destructor
    ~EventIDFilter(){}

    void configure(const larcv3::PSet&);

    void initialize();

    bool process(larcv3::IOManager& mgr);

    void finalize();

  private:


    std::map<larcv3::EventID,bool> _id_m;
    std::string _ref_producer;
    std::string _ref_type;
    bool _remove_duplicate;
    std::string _fname;
    // int _run;
    // int _subrun;
    // int _event;
  };

  /**
     \class larcv3::EventIDFilterFactory
     \brief A concrete factory class for larcv3::EventIDFilter
  */
  class EventIDFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EventIDFilterProcessFactory() { ProcessFactory::get().add_factory("EventIDFilter",this); }
    /// dtor
    ~EventIDFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EventIDFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

