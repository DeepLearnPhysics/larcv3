#ifndef __LARCV_EVENTBASE_CXX
#define __LARCV_EVENTBASE_CXX

#include "EventBase.h"
// #include <sstream>
// #include <iomanip>

namespace larcv3{
    EventBase::~EventBase(){
        for (auto & p : _data_types){
            delete p;
        }
    }
}
#endif
