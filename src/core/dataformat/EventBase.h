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
    
    void clear() {};
#ifndef SWIG
    void initialize(H5::Group *) {};
    void serialize(H5::Group * group) {};
    void deserialize(H5::Group * group, size_t entry) {};
#endif
  };
}

#endif //inc guard
/** @} */ // end of doxygen group 

