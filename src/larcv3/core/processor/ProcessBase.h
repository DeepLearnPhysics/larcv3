/**
 * \file ProcessBase.h
 *
 * \ingroup core_Processor
 *
 * \brief Class def header for a class larcv3::ProcessBase
 *
 * @author drinkingkazu
 */

/** \addtogroup core_Processor

    @{*/
#ifndef __LARCV3PROCESSOR_PROCESSBASE_H
#define __LARCV3PROCESSOR_PROCESSBASE_H

#include "larcv3/core/base/Watch.h"
#include "larcv3/core/dataformat/IOManager.h"
#include "larcv3/core/processor/ProcessorTypes.h"

namespace larcv3 {

  class ProcessDriver;
  class ProcessFactory;
  /**
     \class ProcessBase
     @brief A base class for "process module" to be run by larcv3::ProcessDriver
     @detail Inherited class must implment 4 pure virtual functions that are called by larcv3::ProcessDriver instance.\n
     ProcessBase::configure(const larcv3::PSet) is called first with the argument passing the configuration parameters.\n
     ProcessBase::initialize() is called after configure. This is where you may want to initialize variables.\n
     ProcessBase::process(larcv3::IOManager&) is called for every event. The argument provides an access to event data.\n
     ProcessBase::finalize() is called after larcv3::ProcessDriver finished looping over all events.\n
  */
  class ProcessBase : public larcv_base {
    friend class ProcessDriver;
    friend class ProcessFactory;

  public:

    /// Default constructor
    ProcessBase(const std::string name="ProcessBase");

    /// Default destructor
    virtual ~ProcessBase(){}

    //
    // Four pure virtual functions that larcv3::ProcessDriver calls and need implementation.
    //
    /// Called first with the argument passing the configuration parameters.
    virtual void configure(const json&) = 0;
    /// Called after configure, this is where you should initialize variables to be stored in an output analysis file.
    virtual void initialize() = 0;
    /// Called per-event, this is where you should implement your per-event action/analysis.
    virtual bool process(IOManager& mgr) = 0;
    /// Called after event loop is over. This is where you can store your histograms etc. to an output analysis file.
    virtual void finalize() = 0;

    //
    // Following functions are
    //
    /// Only for experts: allows a loose grouping for a set of ProcessBase inherit classes via true/false return to a "question".
    virtual bool is(const std::string question) const;

    /**
     * @brief      Default configuration
     *
     * @return     JSON default config
     */
    static json default_config(){
      json c = {
          {"ProcessName", ""},
          {"ProcessType", ""},
      };
      return c;
    }


  private:

    void _configure_(const json&);

    bool _process_(IOManager& mgr);

    bool _event_creator;    ///< special flag to mark this algorithm an event creator
    larcv3::Watch _watch;    ///< algorithm profile stopwatch
    double _proc_time;      ///< algorithm execution time record (cumulative)
    size_t _proc_count;     ///< algorithm execution counter (cumulative)

    larcv3::ProcessID_t _id; ///< unique algorithm identifier
    bool _profile;          ///< measure process time if profile flag is on
    std::string _typename;  ///< process type from factory
  };
}
#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_processbase(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group
