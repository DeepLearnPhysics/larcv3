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
#pragma once

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

  class AbstractEventBase{
  public:
    AbstractEventBase(){};
    ~AbstractEventBase(){};
  };


  // Template over the specific data product used.
  template<class dataproduct>
  class EventBase : public AbstractEventBase {
    friend class IOManager;
    friend class DataProductFactory;
  public:

    /// Destructor
    ~EventBase();

    /**
     * @brief      index access pass through
     *
     * @param[in]  index  The index
     *
     * @return     { Returns the object at the index requested }
     */
    inline dataproduct at(size_t index) {return _object_v.at(index);}



    /**
     * @brief      Set the entire collection for this event
     *
     * @param[in]  object_v  The object vector
     */
    void set(const std::vector<dataproduct>& object_v);

    /**
     * @brief      append an object to the end
     *
     * @param[in]  object  The dataproduct
     */
    void append(const dataproduct& object);

    /**
     * @brief      Emplace an object to the end with std::move(dataproduct)
     *
     * @param      object  The object
     */
    void emplace_back(dataproduct&& object);


    /**
     * @brief      emplace a set of objects to the end with std::move
     *
     * @param      object_v  The object v
     */
    void emplace(std::vector<dataproduct>&& object_v);

    inline const std::vector<dataproduct>& as_vector() const
    { return _object_v; }

    // inline const larcv3::Particle& at(size_t index) const {return _object_v.at(index);}

    inline size_t size() const {return _object_v.size();}



    /**
     * @brief      Clears the object.
     */
    void clear();

    /**
     * @brief      Initialization
     *
     * @param[in]  group        The group
     * @param[in]  compression  The compression
     */
    void initialize (hid_t group, uint compression) ;

    /**
     * @brief      Serialize this object
     *
     * @param[in]  group  The group
     */
    void serialize  (hid_t group);

    /**
     * @brief      Deserialize this object at specified entry
     *
     * @param[in]  group          The group
     * @param[in]  entry          The entry
     * @param[in]  reopen_groups  For re-opening groups, if file changed for example
     */
    void deserialize(hid_t group, size_t entry, bool reopen_groups = false);

    /**
     * @brief      Finalize this series of objects by closing HDF5 objects
     */
    void finalize();

    /**
     * @brief      Opens in datasets.
     *
     * @param[in]  group  The group
     */
    void open_in_datasets(hid_t group );
    
    /**
     * @brief      Opens out datasets.
     *
     * @param[in]  group  The group
     */
    void open_out_datasets(hid_t group );

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

  private:
    std::vector<dataproduct> _object_v; ///< a collection of the datatypes (index maintained)

    // The following are write-once, use-often helper functions to simplify the IO layers

    // This is a helper function to register the dataproducts needed for this class:
    // void register_dtypes();

    

// #endif
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>


// template <class dataproduct>
// void init_eventbase(pybind11::module m)
// {
//     using Class = larcv3::EventBase<dataproduct>;

//     pybind11::class_<Class, larcv3::AbstractEventBase> event_base(
//         m, larcv3::product_unique_name<Class>());

//     event_base.def(pybind11::init<>());

//     event_base.def("set",             &Class::set);
//     event_base.def("append",          &Class::append);
//     // event_base.def("emplace_back", &Class::emplace_back);
//     event_base.def("at",              &Class::at);
//     event_base.def("as_vector",       &Class::as_vector);
//     event_base.def("size",            &Class::size);
//     event_base.def("clear",           &Class::clear);

// }
#endif

#include "EventBase.hxx"

/** @} */ // end of doxygen group
