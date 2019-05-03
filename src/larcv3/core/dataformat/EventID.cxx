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

#endif
