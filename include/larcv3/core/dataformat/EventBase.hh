/**
 * @file EventBase.h
 * @ingroup DataFormat
 * @brief Class def header for a class EventBase
 * @author cadams, kazuhiro
 */
#pragma once

#ifdef LARCV_INTERNAL
  #include <pybind11/pybind11.h>
  void init_eventbase(pybind11::module m);
#endif

#include <iostream>

#include "larcv_base.hh"
#include "DataFormatTypes.hh"

/** @addtogroup DataFormat
 * @{
 */
namespace larcv3 
{
	/**
	 * @brief Base class for an event data product 
	 * (what is stored in output file), 
	 * holding run/subrun/event ID + producer name.
	 * There is no need to generate an explicit wrapper for EventBase,
	 * since it's virtual.
	 */
	class EventBase
	{
		friend class IOManager;
		friend class DataProductFactory;
	public:
		/// virtual destructor
		virtual ~EventBase() = 0;

		/// five functions which must be defined
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
	};
}
/** @} */ // end of doxygen group