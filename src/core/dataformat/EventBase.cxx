#ifndef __LARCV_EVENTBASE_CXX
#define __LARCV_EVENTBASE_CXX

#include "EventBase.h"
// #include <sstream>
// #include <iomanip>

namespace larcv3{
    EventBase::~EventBase(){
        // for (auto & p : _data_types){
        //     delete p;
        // }
    }

    int EventBase::get_num_objects(hid_t group){
        hsize_t  num_objects[1] = {0};
        H5Gget_num_objs(group, num_objects);
        return num_objects[0];
    }
}

void init_eventbase(pybind11::module m){
    using Class = larcv3::EventBase;
    pybind11::class_<Class, std::shared_ptr<Class>> base(m, "EventBase");

    // We don't ever want to end up with an event base in python,
    // So nothing is wrapped, not even the constructor.
}

#endif
