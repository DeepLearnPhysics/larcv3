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
#include "larcv/core/base/larcv_base.h"
#include "DataFormatTypes.h"

// There is no need to generate an explicit wrapper for EventBase, since it's virtual
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
    virtual EventBase() = 0;

    /// Default destructor
    virtual ~EventBase() = 0;
    /// Set all run/subrun/event to kINVALID_SIZE

    void clear() {};
    virtual void initialize(H5::Group *) = 0;
    virtual void serialize(H5::Group * group) = 0;
    virtual void deserialize(H5::Group * group, size_t entry) = 0;

  };
}
#endif

#endif //inc guard
/** @} */ // end of doxygen group 

