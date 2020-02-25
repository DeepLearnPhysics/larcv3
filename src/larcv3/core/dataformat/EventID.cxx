#ifndef __LARCV3DATAFORMAT_EVENTID_CXX
#define __LARCV3DATAFORMAT_EVENTID_CXX

#include "larcv3/core/dataformat/EventID.h"
#include <sstream>
#include <iomanip>
namespace larcv3 {

  void EventID::clear()
  { _run = _subrun = _event = kINVALID_SIZE; }
   
  
  std::string EventID::event_key() const
  {
    std::stringstream ss;
    ss << std::setw( 7 ) << std::setfill( '0' ) << _run << "_"
       << std::setw( 5 ) << std::setfill( '0' ) << _subrun << "_"
       << std::setw( 6 ) << std::setfill( '0' ) << _event;
    return ss.str();
  }

}

#include <pybind11/operators.h>

void init_eventid(pybind11::module m){
    using Class = larcv3::EventID;
    pybind11::class_<Class> eventid(m, "EventID");
    eventid.def(pybind11::init<>());
    eventid.def(pybind11::init<const Class& > ());
    eventid.def(pybind11::self == pybind11::self);
    eventid.def(pybind11::self != pybind11::self);
    eventid.def(pybind11::self < pybind11::self);

    eventid.def("run",    (long (Class::*)(    ) const)(&Class::run));
    eventid.def("run",    (void (Class::*)(long))(&Class::run));
    eventid.def("subrun", (long (Class::*)(    ) const)(&Class::subrun));
    eventid.def("subrun", (void (Class::*)(long))(&Class::subrun));
    eventid.def("event",  (long (Class::*)(    ) const)(&Class::event));
    eventid.def("event",  (void (Class::*)(long))(&Class::event));

    eventid.def("clear",     &Class::clear);
    eventid.def("valid",     &Class::valid);
    eventid.def("set_id",    &Class::set_id);
    eventid.def("event_key", &Class::event_key);
    eventid.def("__repr__",  &Class::event_key);

/*


  /// Formatted string key getter (a string key consists of 0-padded run,
  /// subrun, and event id)
  std::string event_key() const;


*/

}


#endif
