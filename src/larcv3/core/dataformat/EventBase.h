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

    /// Destructor
    virtual ~EventBase() = 0;

    /**
     * @brief      Clears the object.
     */
    virtual void clear() = 0;

    /**
     * @brief      Initialization
     *
     * @param[in]  group        The group
     * @param[in]  compression  The compression
     */
    virtual void initialize (hid_t group, uint compression) = 0;

    /**
     * @brief      Serialize this object
     *
     * @param[in]  group  The group
     */
    virtual void serialize  (hid_t group) = 0;

    /**
     * @brief      Deserialize this object at specified entry
     *
     * @param[in]  group          The group
     * @param[in]  entry          The entry
     * @param[in]  reopen_groups  For re-opening groups, if file changed for example
     */
    virtual void deserialize(hid_t group, size_t entry, bool reopen_groups) = 0;

    /**
     * @brief      Finalize this series of objects by closing HDF5 objects
     */
    virtual void finalize() = 0;

    /**
     * @brief      Opens in datasets.
     *
     * @param[in]  group  The group
     */
    virtual void open_in_datasets(hid_t group ) = 0;
    
    /**
     * @brief      Opens out datasets.
     *
     * @param[in]  group  The group
     */
    virtual void open_out_datasets(hid_t group ) = 0;

    std::vector<hid_t> _open_in_datasets;
    std::vector<hid_t> _open_in_dataspaces;
    std::vector<hid_t> _open_out_datasets;
    std::vector<hid_t> _open_out_dataspaces;
    std::vector<hid_t> _data_types;

    /**
     * @brief      Gets the number objects.
     *
     * @param[in]  group  The group
     *
     * @return     The number objects.
     */
    int get_num_objects(hid_t group);



// #endif
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_eventbase(pybind11::module m);
#endif

#endif //inc guard
/** @} */ // end of doxygen group
