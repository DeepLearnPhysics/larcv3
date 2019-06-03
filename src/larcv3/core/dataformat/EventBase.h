/**
 * \file EventBase.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventBase
 *
 * @author cadams, kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_EVENTBASE_H
#define __LARCV3DATAFORMAT_EVENTBASE_H

#include <iostream>
#include "larcv3/core/base/larcv_base.h"
#include "larcv3/core/dataformat/DataFormatTypes.h"

// There is no need to generate an explicit wrapper for EventBase, since it's virtual

namespace larcv3 {
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
    
    virtual ~EventBase() = 0;

    virtual void clear() = 0;
    virtual void initialize(H5::Group *) = 0;
    virtual void serialize(H5::Group * group) = 0;
    virtual void deserialize(H5::Group * group, size_t entry) = 0;

    virtual void open_datasets(H5::Group * group ) = 0;

    std::vector<H5::DataSet>   _open_datasets;
    std::vector<H5::DataSpace> _open_dataspaces;

// #endif
  };

}

#endif //inc guard
/** @} */ // end of doxygen group 

