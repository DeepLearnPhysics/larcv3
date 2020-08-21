#ifndef __LARCV3PROCESSOR_PROCESSBASE_CXX
#define __LARCV3PROCESSOR_PROCESSBASE_CXX

#include "larcv3/core/processor/ProcessBase.h"

namespace larcv3 {

  ProcessBase::ProcessBase(const std::string name)
    : larcv_base  ( name )
    , _proc_time  ( 0. )
    , _proc_count ( 0  )
    , _id         ( kINVALID_SIZE )
    , _profile    ( false )
  {}

  bool ProcessBase::is(const std::string question) const
  { return false; }

  void ProcessBase::_configure_(const json& cfg)
  {
    LARCV_NORMAL() << "config is " << cfg << std::endl;
    _profile = false;
    if (cfg.contains("Profile")){
      _profile = cfg["Profile"].get<bool>();
    }
    configure(cfg);
  }
  
  bool ProcessBase::_process_(IOManager& mgr)
  {
    bool status=false;
    if(_profile) {
      _watch.Start();
      status = this->process(mgr);
      _proc_time += _watch.WallTime();
      ++_proc_count;
    }else status = this->process(mgr);
    return status;
  }


}

void init_processbase(pybind11::module m){
    using Class = larcv3::ProcessBase;
    pybind11::class_<Class> processbase(m, "ProcessBase");
    
    // We don't actually want to construct any instances of ProcessBase
    // processbase.def(pybind11::init<const std::string>(), 
                    // pybind11::arg("name")="ProcessBase");

}


#endif
