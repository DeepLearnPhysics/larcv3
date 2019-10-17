/**
 * \file EventID.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class EventID
 *
 * @author kazuhiro, cadams
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_EVENTID_H
#define __LARCV3DATAFORMAT_EVENTID_H

#include <iostream>
#include "hdf5.h"
#include "larcv3/core/dataformat/DataFormatTypes.h"

namespace larcv3 {
// class IOManager;
class DataProductFactory;
/**
  \class EventID
  Base class for an event data product (what is stored in output file), holding
  run/subrun/event ID + producer name.
*/
class EventID {
 public:
  /// Default constructor
  EventID() = default;
  /// Copy ctor
  EventID(const EventID& rhs)
      : _run(rhs._run), _subrun(rhs._subrun), _event(rhs._event) {}

  /// Default destructor
  ~EventID() = default;
  /// Set all run/subrun/event to kINVALID_LONG
  void clear();
  /// Run number getter
  long run() const { return _run; }
  /// SubRun number getter
  long subrun() const { return _subrun; }
  /// Event number getter
  long event() const { return _event; }
  /// Run number setter
  inline void run(long _run) { this->_run = _run; }
  /// SubRun number setter
  inline void subrun(long _subrun) { this->_subrun = _subrun; }
  /// Event number setter
  inline void event(long _event) { this->_event = _event; }

  /// Make sure run/subrun/event ID is set
  bool valid() const {
    return !(_run == kINVALID_LONG || _subrun == kINVALID_LONG ||
             _event == kINVALID_LONG);
  }
  /// Comparison opearator for run/subrun/event id
  inline bool operator==(const EventID& rhs) const {
    return (_run == rhs.run() && _subrun == rhs.subrun() &&
            _event == rhs.event());
  }
  /// Comparison opearator for run/subrun/event id
  inline bool operator!=(const EventID& rhs) const { return !((*this) == rhs); }
  /// Comparison opearator for run/subrun/event id (for sorted container like
  /// std::set and/or std::map)
  inline bool operator<(const EventID& rhs) const {
    if (_run < rhs.run()) return true;
    if (_run > rhs.run()) return false;
    if (_subrun < rhs.subrun()) return true;
    if (_subrun > rhs.subrun()) return false;
    if (_event < rhs.event()) return true;
    if (_event > rhs.event()) return false;
    return false;
  }

  /// Formatted string key getter (a string key consists of 0-padded run,
  /// subrun, and event id)
  std::string event_key() const;

  /// Setting the id
  inline void set_id(const long run, const long subrun,
                     const long event) {
    _run = run;
    _subrun = subrun;
    _event = event;
  }

#ifndef SWIG
  public: 
    static hid_t get_datatype() {
      hid_t datatype;
      herr_t status;
      datatype = H5Tcreate (H5T_COMPOUND, sizeof (EventID));
      status = H5Tinsert (datatype, "run",
                  HOFFSET (EventID, _run), larcv3::get_datatype<long>());
      status = H5Tinsert (datatype, "subrun", 
                  HOFFSET (EventID, _subrun), larcv3::get_datatype<long>());
      status = H5Tinsert (datatype, "event",
                  HOFFSET (EventID, _event), larcv3::get_datatype<long>());
      return datatype;
    }
#endif

 private:
  long _run;     ///< LArSoft run number
  long _subrun;  ///< LArSoft sub-run number
  long _event;   ///< LArSoft event number
};
}  // namespace larcv3

#endif
/** @} */  // end of doxygen group
