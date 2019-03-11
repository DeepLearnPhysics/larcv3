/**
 * \file EventBase.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventBase
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV_EVENTBASE_H
#define __LARCV_EVENTBASE_H

#include <iostream>
#include "DataFormatTypes.h"

// There is no need to generate an explicit wrapper for EventBase:
#ifndef SWIG

namespace larcv {
  // class IOManager;
  class DataProductFactory;
  /**
    \class EventBase
    Base class for an event data product (what is stored in output file), holding run/subrun/event ID + producer name.
  */
  class EventBase{
    friend class IOManager;
    friend class DataProductFactory;
  public:
    
    /// Default constructor
    EventBase() = default;

    /// Default destructor
    ~EventBase() = default;
    /// Set all run/subrun/event to kINVALID_SIZE

    virtual void clear() = 0;
    virtual void serialize(H5::Group * group) = 0;


  };
}
#endif //swig

#endif //inc guard
/** @} */ // end of doxygen group 

