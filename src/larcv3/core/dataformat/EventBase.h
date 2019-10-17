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
    virtual void initialize (hid_t group, uint compression) = 0;
    virtual void serialize  (hid_t group) = 0;
    virtual void deserialize(hid_t group, size_t entry, bool reopen_groups) = 0;
    virtual void finalize() = 0;

    virtual void open_in_datasets(hid_t group ) = 0;
    virtual void open_out_datasets(hid_t group ) = 0;

    std::vector<hid_t> _open_in_datasets;
    std::vector<hid_t> _open_in_dataspaces;
    std::vector<hid_t> _open_out_datasets;
    std::vector<hid_t> _open_out_dataspaces;
    std::vector<hid_t> _data_types;

    int get_num_objects(hid_t group);

// #endif
  };

}

#endif //inc guard
/** @} */ // end of doxygen group 

