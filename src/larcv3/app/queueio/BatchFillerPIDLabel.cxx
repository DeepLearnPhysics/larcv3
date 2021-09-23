#ifndef __LARCV3THREADIO_BATCHFILLERPIDLABEL_CXX__
#define __LARCV3THREADIO_BATCHFILLERPIDLABEL_CXX__

#include "BatchFillerPIDLabel.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventParticle.h"
#include <random>

namespace larcv3 {

  static BatchFillerPIDLabelProcessFactory __global_BatchFillerPIDLabelProcessFactory__;

  BatchFillerPIDLabel::BatchFillerPIDLabel(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerPIDLabel::configure(const json& cfg)
  {
    config = this -> default_config();
    config = augment_default_config(config, cfg);



    auto _pdg_list = cfg["PdgClassList"].get<std::vector<int> >();
    if (_pdg_list.empty()) {
      LARCV_CRITICAL() << "PdgClassList needed to define classes!" << std::endl;
      throw larbys();
    }
  }

  void BatchFillerPIDLabel::initialize()
  {}

  void BatchFillerPIDLabel::_batch_begin_()
  {
    if (!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }
  }

  void BatchFillerPIDLabel::_batch_end_()
  {

  }

  void BatchFillerPIDLabel::finalize()
  {}

  bool BatchFillerPIDLabel::process(IOManager & mgr)
  {

    std::string producer = config["Producer"].get<std::string>();
    auto const& event_part = mgr.get_data<larcv3::EventParticle>(producer);
    auto _pdg_list = config["PdgClassList"].get<std::vector<int> >();

    // Refresh the dimension:
    std::vector<int> dim(2);
    dim[0] = batch_size();
    dim[1] = _pdg_list.size();
    set_dim(dim);
    // In this case, set the dense dim as the same:
    set_dense_dim(dim);

    // labels
    auto const& part_v = event_part.as_vector();
    if (part_v.size() != 1) {
      LARCV_CRITICAL() << "Only support single particle label now: EventParticle size != 1" << std::endl;
      throw larbys();
    }
    // class
    size_t label = kINVALID_SIZE;
    int pdg = 0;
    for (auto const& part : part_v) {
      for (size_t class_idx = 0; class_idx < _pdg_list.size(); ++class_idx) {
        pdg = part.pdg_code();
        if (pdg != _pdg_list[class_idx]) continue;
        label = class_idx;
        break;
      }
      if (label != kINVALID_SIZE) break;
    }
    LARCV_DEBUG() << "Found PDG code " << pdg << " (class=" << label << ")" << std::endl;
    _entry_data.resize(_pdg_list.size(), 0);
    for (auto& v : _entry_data) v = 0;
    _entry_data.at(label) = 1.;


    set_entry_data(_entry_data);

    return true;
  }

}

#include <typeinfo>
#include <pybind11/stl.h>

void init_bf_pid(pybind11::module m){

    using Class = larcv3::BatchFillerPIDLabel;
    std::string classname = "BatchFillerPIDLabel";
    pybind11::class_<Class> batch_filler(m, classname.c_str());
    batch_filler.def(pybind11::init<>());

    // batch_filler.def("pydata",             &Class::pydata);
    batch_filler.def("default_config",     &Class::default_config);
    batch_filler.def("process",            &Class::process);
    

}

#endif
