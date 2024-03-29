#ifndef __LARCV3PROCESSOR_PROCESSDRIVER_CXX
#define __LARCV3PROCESSOR_PROCESSDRIVER_CXX

#include "larcv3/core/processor/ProcessDriver.h"
#include <iomanip>
#include <random>
#include <sstream>
#include "larcv3/core/processor/ProcessFactory.h"
#include "larcv3/core/base/LArCVBaseUtilFunc.h"
namespace larcv3 {

ProcessDriver::ProcessDriver(std::string name)
    : larcv_base(name),
      _proc_v(),
      _processing(false) {
      }

void ProcessDriver::reset() {
  LARCV_DEBUG() << "Called" << std::endl;
  _io.reset();
  for (size_t i = 0; i < _proc_v.size(); ++i) {
    delete _proc_v[i];
    _proc_v[i] = nullptr;
  }
  config = json();
  _proc_v.clear();
  _processing = false;
}

void ProcessDriver::override_input_file(const std::vector<std::string>& flist) {
  LARCV_DEBUG() << "Called" << std::endl;
  _io.clear_in_file();
  for (auto const& f : flist) _io.add_in_file(f);
}

void ProcessDriver::override_output_file(const std::string fname) {
  LARCV_DEBUG() << "Called" << std::endl;
  _io.set_out_file(fname);
}


ProcessID_t ProcessDriver::process_id(std::string name) const {
  LARCV_DEBUG() << "Called" << std::endl;
  auto iter = _proc_m.find(name);
  if (iter == _proc_m.end()) {
    LARCV_CRITICAL() << "Process w/ name " << name << " not found..."
                     << std::endl;
    throw larbys();
  }
  return (*iter).second;
}

std::vector<std::string> ProcessDriver::process_names() const {
  LARCV_DEBUG() << "Called" << std::endl;
  std::vector<std::string> res;
  res.reserve(_proc_m.size());
  for (auto const& name_id : _proc_m) res.push_back(name_id.first);
  return res;
}

const std::map<std::string, larcv3::ProcessID_t>& ProcessDriver::process_map()
    const {
  LARCV_DEBUG() << "Called" << std::endl;
  return _proc_m;
}

const ProcessBase* ProcessDriver::process_ptr(size_t id) const {
  LARCV_DEBUG() << "Called" << std::endl;
  if (id >= _proc_v.size()) {
    LARCV_CRITICAL() << "Invalid ID requested: " << id << std::endl;
    throw larbys();
  }
  return _proc_v[id];
}


void ProcessDriver::configure(const json& cfg) {
  reset();

  // Grab the default config:
  config = this->default_config();


  config = augment_default_config(config, cfg);



  // Set the verbosity up front: 
  set_verbosity(config["Verbosity"].get<msg::Level_t>());
  larcv3::logger::get_shared().set(logger().level());

  // // check io config exists
  // LARCV_INFO() << "Retrieving IO config" << std::endl;
  // json io_config("Empty");
  // if (cfg.contains_pset("IOManager"))
  //   io_config = cfg.get<larcv3::PSet>("IOManager");
  // else if (cfg.contains_pset(std::string(name() + "IOManager")))
  //   io_config = cfg.get<larcv3::PSet>(name() + "IOManager");
  // else {
  //   LARCV_CRITICAL() << "IOManager config not found!" << std::endl
  //                    << cfg.dump() << std::endl;
  //   throw larbys();
  // }
  /// CJA, 8/10/20: IOManager is filled in by default now, so this check isn't necessary.



  // // check process config exists
  // LARCV_INFO() << "Retrieving ProcessList" << std::endl;
  // if (!cfg.contains("ProcessList")) {
  //   LARCV_CRITICAL() << "ProcessList config not found!" << std::endl
  //                    << cfg.dump() << std::endl;
  //   throw larbys();
  // }
  /// CJA, 8/10/20: ProcessList is in the default now, so this check isn't necessary.



  auto const io_config   = config["IOManager"].get<json>();
  auto const proc_config = config["ProcessList"].get<json>();


  // Prepare IO manager
  LARCV_INFO() << "Configuring IO" << std::endl;
  _io.configure(io_config);
  // Set ProcessDriver
  LARCV_INFO() << "Retrieving self (ProcessDriver) config" << std::endl;
  

  LARCV_INFO() << "Enable Filter is :  " << config["EnableFilter"].get<bool>() << std::endl;
  auto random_access_bool = config["RandomAccess"].get<bool>();
  LARCV_INFO() << "RandomAccess is :  " << random_access_bool << std::endl;


  // Process list
  auto process_instance_type_v =
      config["ProcessType"].get<std::vector<std::string> >();
  auto process_instance_name_v =
      config["ProcessName"].get<std::vector<std::string> >();

  if (process_instance_type_v.size() != process_instance_name_v.size()) {
    LARCV_CRITICAL() << "ProcessType and ProcessName config parameters have "
                        "different length! "
                     << "(" << process_instance_type_v.size() << " vs. "
                     << process_instance_name_v.size() << ")" << std::endl;
    throw larbys();
  }

  LARCV_INFO() << "Start looping process list to instantiate processes"
               << std::endl;
  for (auto& p : _proc_v)
    if (p) {
      delete p;
    }
  _proc_v.clear();
  _proc_m.clear();
  for (size_t i = 0; i < process_instance_type_v.size(); ++i) {
    auto const& name = process_instance_name_v[i];
    auto const& type = process_instance_type_v[i];
    if (_proc_m.find(name) != _proc_m.end()) {
      LARCV_CRITICAL() << "Duplicate Process name found: " << name << std::endl;
      throw larbys("Duplicate algorithm name found!");
    }
    size_t id = _proc_v.size();
    LARCV_NORMAL() << "Instantiating Process ID=" << id << " Type: " << type
                   << " w/ Name: " << name << std::endl;
    /*
    if(!proc_config.contains_pset(name)) {
      LARCV_CRITICAL() << "Could not locate configuration for " << name <<
    std::endl
                       << proc_config.dump() << std::endl;
      throw larbys();
    }
    */
    auto ptr = ProcessFactory::get().create(type, name);
    ptr->_id = id;
    ptr->_configure_(proc_config[name].get<json>());
    _proc_m[name] = id;

    _proc_v.push_back(ptr);
  }
}

void ProcessDriver::initialize(int color) {
  LARCV_DEBUG() << "Called" << std::endl;
  // check state
  if (_processing) {
    LARCV_CRITICAL() << "Must call finalize() before calling initialize() "
                        "after starting to process..."
                     << std::endl;
    throw larbys();
  }

  // Initialize IO
  LARCV_INFO() << "Initializing IO " << std::endl;
  _io.initialize(color);

  // Handle invalid cases
  auto const nentries = _io.get_n_entries();
  auto const io_mode = _io.io_mode();

  // Random access + write mode cannot be combined
  if (config["RandomAccess"].get<bool>() != 0 && io_mode == IOManager::kWRITE) {
    LARCV_CRITICAL() << "Random access mode not supported for kWRITE IO mode!"
                     << std::endl;
    throw larbys();
  }
  // No entries means nothing to do unless write mode
  if (!nentries && io_mode != IOManager::kWRITE) {
    LARCV_CRITICAL()
        << "No entries found from IO (kREAD/kBOTH mode cannot be run)!"
        << std::endl;
    throw larbys();
  }

  // Initialize process
  for (auto& p : _proc_v) {
    LARCV_INFO() << "Initializing: " << p->name() << std::endl;
    p->initialize();
  }

  // Change state from to-be-initialized to to-process
  _processing = true;

  // Prepare ttree entry index array to follow in execution (randomize if
  // specified
  LARCV_INFO() << "Preparing access index vector" << std::endl;
  if (nentries) {
    _access_entry_v.resize(nentries);
    for (size_t i = 0; i < _access_entry_v.size(); ++i) _access_entry_v[i] = i;
    // if(config["RandomAccess"].get<bool>())
    // std::random_shuffle(_access_entry_v.begin(),_access_entry_v.end());
    if (config["RandomAccess"].get<bool>()) {
      unsigned int seed = 0;
      if (config["RandomSeed"].get<int>() < 0)
        seed = std::chrono::system_clock::now().time_since_epoch().count();
      else
        seed = config["RandomSeed"].get<int>();
      std::shuffle(_access_entry_v.begin(), _access_entry_v.end(),
                   std::default_random_engine(seed));
    }
  }

  _current_entry = 0;
}

bool ProcessDriver::_process_entry_() {
  // Private method to execute processes and change entry number record
  // This method does not perform any sanity check, hence private and
  // should be used by wrapper method which performs necessary checks.

  // Execute
  bool good_status = true;
  // bool cleared=false;
  for (auto& p : _proc_v) {
    good_status = good_status && p->_process_(_io);
    if (!good_status && config["EnableFilter"].get<bool>() ) break;
  }
  // If not read mode save entry
  if (_io.io_mode() != IOManager::kREAD && (!config["EnableFilter"].get<bool>()  || good_status)) {
    // cleared = true;
    _io.save_entry();
  }

   
  // Bump up entry record
  ++_current_entry;

  return good_status;
}

bool ProcessDriver::process_entry() {
  LARCV_DEBUG() << "Called" << std::endl;
  // Public method to process "next" entry

  // Check state
  if (!_processing) {
    LARCV_CRITICAL() << "Must call initialize() before start processing!"
                     << std::endl;
    throw larbys();
  }

  // Check if input entry exists in case of read/both io mode
  if (_io.io_mode() != IOManager::kWRITE) {
    if (_access_entry_v.size() <= _current_entry) {
      LARCV_NORMAL() << "Entry " << _current_entry
                     << " exceeds available events in a file!" << std::endl;
      return false;
    }
    // if exist then move read pointer
    //_io.clear_entry();
    _io.read_entry(_access_entry_v[_current_entry]);
  }
  // Execute processes
  return _process_entry_();
}

bool ProcessDriver::process_entry(size_t entry, bool force_reload) {
  LARCV_DEBUG() << "Called" << std::endl;
  // Public method to process "specified" entry

  // Check state
  if (!_processing) {
    LARCV_CRITICAL() << "Must call initialize() before start processing!"
                     << std::endl;
    throw larbys();
  }

  // Check if input entry exists in case of read/both io mode
  if (_io.io_mode() != IOManager::kWRITE) {
    if (_access_entry_v.size() <= entry) {
      LARCV_ERROR() << "Entry " << entry
                    << " exceeds available events in a file!" << std::endl;
      return false;
    }
    // if exist then move read pointer
    //_io.clear_entry();
    _io.read_entry(_access_entry_v[entry], force_reload);
    _current_entry = entry;
  }
  // Execute processes
  return _process_entry_();
}

void ProcessDriver::batch_process(size_t start_entry, size_t num_entries) {
  LARCV_DEBUG() << "Called" << std::endl;
  // Public method to execute num_entries starting from start_entry
  if (!start_entry) start_entry = config["StartEntry"].get<int>();
  if (!num_entries) num_entries = config["NumEntries"].get<int>();

  // Check state
  if (!_processing) {
    LARCV_CRITICAL() << "Must call initialize() before start processing!"
                     << std::endl;
    throw larbys();
  }

  // Define the max entry index for processing (1 above last entry to be
  // processed)
  size_t max_entry = start_entry + num_entries;

  // Check if start_entry is 0 for write mode (no entry should be specified for
  // write mode)
  if (_io.io_mode() == IOManager::kWRITE) {
    if (start_entry) {
      LARCV_CRITICAL()
          << "Cannot specify start entry (1st arg) in kWRITE IO mode!"
          << std::endl;
      throw larbys();
    }
    max_entry = _current_entry + num_entries;

  } else {
    _current_entry = start_entry;
    if (!num_entries) max_entry = start_entry + _io.get_n_entries();
  }

  // Make sure max entry does not exceed the physical max from input. If so,
  // truncate.
  if (_io.io_mode() != IOManager::kWRITE &&
      max_entry > _access_entry_v.size()) {
    LARCV_WARNING() << "Requested to process entries from " << start_entry
                    << " to " << max_entry - 1 << " ... but there are only "
                    << _access_entry_v.size() << " entries in input!"
                    << std::endl
                    << "Truncating the end entry to "
                    << _access_entry_v.size() - 1 << std::endl;
    max_entry = _access_entry_v.size();
  }

  // Batch-execute in while loop
  size_t num_processed = 0;
  size_t num_fraction = (max_entry - _current_entry) / 10;
  while (_current_entry < max_entry) {
    if (_io.io_mode() != IOManager::kWRITE) {
      //_io.clear_entry();
      _io.read_entry(_access_entry_v[_current_entry]);
    }
    _process_entry_();

    ++num_processed;
    if (!num_fraction) {
      LARCV_NORMAL() << "Processed " << num_processed << " entries..."
                     << std::endl;
    } else if (num_processed % num_fraction == 0) {
      LARCV_NORMAL() << "Processed " << 10 * int(num_processed / num_fraction)
                     << " %..." << std::endl;
    }
  }
}

void ProcessDriver::finalize() {
  LARCV_DEBUG() << "called" << std::endl;

  for (auto& p : _proc_v) {
    LARCV_INFO() << "Finalizing: " << p->name() << std::endl;
    p->finalize();
  }

  // Profile repor
  LARCV_INFO() << "Compiling time profile..." << std::endl;
  std::stringstream ss;
  for (auto& p : _proc_v) {
    if (!p->_profile) continue;
    ss << "  \033[93m" << std::setw(20) << std::setfill(' ') << p->name()
       << "\033[00m"
       << " ... # call " << std::setw(5) << std::setfill(' ') << p->_proc_count
       << " ... total time " << p->_proc_time << " [s]"
       << " ... average time " << p->_proc_time / p->_proc_count
       << " [s/process]" << std::endl;
  }

  std::string msg(ss.str());
  if (!msg.empty())
    LARCV_NORMAL() << "Simple time profiling requested and run..." << std::endl
                   << "  ================== " << name()
                   << " Profile Report ==================" << std::endl
                   << msg << std::endl;

  LARCV_INFO() << "Finalizing IO..." << std::endl;
  _io.finalize();
  LARCV_INFO() << "Resetting..." << std::endl;
  reset();
}

size_t ProcessDriver::get_tree_index(size_t entry) const {
  if (entry < _access_entry_v.size())
    return _access_entry_v.at(entry);
  else
    throw larbys();
  return 0;
}

}  // namespace larcv3

#include <pybind11/stl.h>

void init_processdriver(pybind11::module m){
    using Class = larcv3::ProcessDriver;
    pybind11::class_<Class> processdriver(m, "ProcessDriver");
    processdriver.def(pybind11::init<const std::string>(), 
                    pybind11::arg("name")   = "ProcessDriver");


    // processdriver.def("configure",            &Class::configure);

    processdriver.def("configure",            &Class::configure);
    processdriver.def("config",               &Class::get_config);
    processdriver.def("get_config",           &Class::get_config);

    processdriver.def("override_input_file",  &Class::override_input_file);
    processdriver.def("override_output_file", &Class::override_output_file);
    processdriver.def("random_access",        &Class::random_access);
    processdriver.def("reset",                &Class::reset);
    processdriver.def("initialize",           &Class::initialize,pybind11::arg("color")=0);
    processdriver.def("batch_process",        &Class::batch_process,
      pybind11::arg("start_entry")=0, pybind11::arg("num_entries")=0);

    processdriver.def("process_entry",
      (bool (Class::*)() )(                   &Class::process_entry));
    processdriver.def("process_entry", 
      (bool (Class::*)( size_t, bool))(       &Class::process_entry),
      pybind11::arg("entry"), pybind11::arg("force_reload")=false);

    processdriver.def("finalize",             &Class::finalize);
    processdriver.def("clear_entry",          &Class::clear_entry);
    processdriver.def("set_id",               &Class::set_id);
    processdriver.def("event_id",             &Class::event_id);
    processdriver.def("process_id",           &Class::process_id);
    processdriver.def("process_names",        &Class::process_names);
    processdriver.def("process_map",          &Class::process_map);
    processdriver.def("process_ptr",          &Class::process_ptr);
    processdriver.def("io",                   &Class::io);
    processdriver.def("get_tree_index",       &Class::get_tree_index);
    processdriver.def("processing",           &Class::processing);

    processdriver.def("default_config",       &Class::default_config);

}


#endif
