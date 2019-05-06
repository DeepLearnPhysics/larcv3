#ifndef __LARCV3FILTER_EVENTIDFILTER_CXX__
#define __LARCV3FILTER_EVENTIDFILTER_CXX__

#include "EventIDFilter.h"
#include "larcv3/core/cpputil/CSVReader.h"

namespace larcv3 {

  static EventIDFilterProcessFactory __global_EventIDFilterProcessFactory__;

  EventIDFilter::EventIDFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void EventIDFilter::configure(const PSet& cfg)
  {
    _ref_producer=cfg.get<std::string>("RefProducer");
    _ref_type=cfg.get<std::string>("RefType");
    _remove_duplicate=cfg.get<int>("RemoveDuplicate",0);
    
    auto file_path=cfg.get<std::string>("CSVFilePath");
    auto format=cfg.get<std::string>("Format","II");
    auto data = larcv3::read_csv(file_path,format);
    auto const& run_v = data.get<int>("run");
    auto const& subrun_v = data.get<int>("subrun");
    auto const& event_v = data.get<int>("event");
    _id_m.clear();
    EventID id;
    for(size_t i=0; i<run_v.size(); ++i) {
      id.set_id( run_v[i], subrun_v[i], event_v[i] );
      _id_m[id] = false;
    }
    LARCV_INFO() << "Registered: " << _id_m.size() << " unique events to be kept..." << std::endl;
  }

  void EventIDFilter::initialize()
  {
  }

  bool EventIDFilter::process(IOManager& mgr)
  {

    EventID ref_id = mgr.event_id();
    // ref_id.set_id(ptr->run(),ptr->subrun(),ptr->event());
    auto itr = _id_m.find(ref_id);

    bool keepit = (itr != _id_m.end());
    LARCV_INFO() << "Event key: " << ref_id.event_key() << " ... keep it? " << keepit << std::endl;
    bool duplicate = false;
    if(keepit) {
      duplicate = (*itr).second;
      if(duplicate) LARCV_WARNING() << "Run " << ref_id.run() << " SubRun " << ref_id.subrun() << " Event " << ref_id.event() << " is duplicated!!!" << std::endl;
      (*itr).second = true;
    }
    if(duplicate && _remove_duplicate) return false;


    return keepit;
  }

  void EventIDFilter::finalize()
  {
    for(auto const& id_used : _id_m) {
      if(id_used.second) continue;
      LARCV_WARNING() << "Event ID not found in data file (unused): Run " << id_used.first.run()
		      << " SubRun " << id_used.first.subrun() 
		      << " Event " << id_used.first.event() << std::endl;
    }
  }

}
#endif
