#ifndef __LARCV3DATAFORMAT_IOMANAGER_CXX__
#define __LARCV3DATAFORMAT_IOMANAGER_CXX__

#include "larcv3/core/dataformat/IOManager.h"
#include <algorithm>
#include "larcv3/core/dataformat/DataProductFactory.h"
#include "assert.h"
#include "larcv3/core/base/LArCVBaseUtilFunc.h"

#define EVENT_ID_CHUNK_SIZE 100
#define EVENT_ID_COMPRESSION 1


#include <mutex>
std::mutex __ioman_mtx;
namespace larcv3 {

IOManager::IOManager(IOMode_t mode, std::string name)
    : larcv_base(name),
      _io_mode(mode),
      _prepared(false),
      _out_index(0),
      _out_entries(0),
      _out_file_name(""),
      _in_index(0),
      _current_offset(0),
      _in_entries_total(0),
      _out_group_v(),
      _in_file_v(),
      _in_dir_v(),
      _key_list(),
      _product_ctr(0),
      _product_ptr_v(),
      _product_type_v(),
      _producer_name_v(),
      _h5_core_driver(false),
      _force_reopen_groups(false) {
  reset();
  _event_id_datatype = larcv3::EventID::get_datatype();
}

IOManager::IOManager(const PSet& cfg)
    : IOManager(kREAD, cfg.get<std::string>("Name")) {
  reset();
  configure(cfg);
}

IOManager::IOManager(std::string config_file, std::string name)
    : IOManager(kREAD, name) {
  // check cfg file
  if (config_file.empty()) {
    LARCV_CRITICAL() << "Config file not set!" << std::endl;
    throw larbys();
  }

  // check cfg content top level
  auto main_cfg = CreatePSetFromFile(config_file);
  if (!main_cfg.contains_pset(name)) {
    LARCV_CRITICAL() << "IOManager configuration (" << name
                     << ") not found in the config file (dump below)"
                     << std::endl
                     << main_cfg.dump() << std::endl;
    throw larbys();
  }
  auto const cfg = main_cfg.get<larcv3::PSet>(name);
  reset();
  configure(cfg);
}

IOManager::~IOManager(){}

void IOManager::add_in_file(const std::string filename,
                            const std::string dirname) {
  _in_file_v.push_back(filename);
  _in_dir_v.push_back(dirname);
}

void IOManager::clear_in_file() { _in_file_v.clear(); }

void IOManager::set_core_driver(const bool opt) { _h5_core_driver = opt; }

void IOManager::set_out_file(const std::string name) { _out_file_name = name; }

std::string IOManager::product_type(const size_t id) const {
  if (id > _product_type_v.size()) {
    LARCV_CRITICAL() << "Product ID " << id << " does not exist... "
                     << std::endl;
    throw larbys();
  }
  return _product_type_v[id];
}

void IOManager::configure(const PSet& cfg) {
  if (_prepared) throw larbys("Cannot call configure() after initialize()!");

  set_verbosity(
      (msg::Level_t)(cfg.get<unsigned short>("Verbosity", logger().level())));
  _io_mode = (IOMode_t)(cfg.get<unsigned short>("IOMode"));
  _out_file_name = cfg.get<std::string>("OutFileName", "");

  _h5_core_driver = cfg.get<bool>("UseH5CoreDriver", false);

  // Figure out input files
  _in_file_v.clear();
  _in_file_v = cfg.get<std::vector<std::string> >("InputFiles", _in_file_v);
  _in_dir_v.clear();
  _in_dir_v = cfg.get<std::vector<std::string> >("InputDirs", _in_dir_v);
  if (_in_dir_v.empty()) _in_dir_v.resize(_in_file_v.size(), "");
  if (_in_dir_v.size() != _in_file_v.size()) {
    LARCV_CRITICAL() << "# of input file (" << _in_file_v.size()
                     << ") != # of input dir (" << _in_dir_v.size() << ")!"
                     << std::endl;
    throw larbys();
  }

  std::vector<std::string> store_only_name;
  std::vector<std::string> store_only_type;
  store_only_name =
      cfg.get<std::vector<std::string> >("StoreOnlyName", store_only_name);
  store_only_type =
      cfg.get<std::vector<std::string> >("StoreOnlyType", store_only_type);
  if (store_only_name.size() != store_only_type.size()) {
    LARCV_CRITICAL() << "StoreOnlyName and StoreOnlyType has different lengths!"
                     << std::endl;
    throw larbys();
  }
  for (size_t i = 0; i < store_only_name.size(); ++i) {
    auto& val = _store_only[store_only_type[i]];
    val.insert(store_only_name[i]);
  }

  std::vector<std::string> read_only_name;
  std::vector<std::string> read_only_type;
  read_only_name =
      cfg.get<std::vector<std::string> >("ReadOnlyName", read_only_name);
  read_only_type =
      cfg.get<std::vector<std::string> >("ReadOnlyType", read_only_type);
  if (read_only_name.size() != read_only_type.size()) {
    LARCV_CRITICAL() << "ReadOnlyName and ReadOnlyType has different lengths!"
                     << std::endl;
    throw larbys();
  }
  for (size_t i = 0; i < read_only_name.size(); ++i) {
    auto& val = _read_only[read_only_type[i]];
    val.insert(read_only_name[i]);
  }
}

bool IOManager::initialize() {
  LARCV_DEBUG() << "start" << std::endl;

  // Lock:
  __ioman_mtx.lock();


  if (_io_mode != kREAD) {
    if (_out_file_name.empty()) throw larbys("Must set output file name!");
    LARCV_INFO() << "Opening an output file: " << _out_file_name << std::endl;

    _out_file = H5::H5File(_out_file_name.c_str(), H5F_ACC_TRUNC);

    // Create the top level groups in the output file.
    _out_file.createGroup("/Events");
    _out_file.createGroup("/Data");

    // Create the output EventID dataset:
    hsize_t starting_dim[] = {0};
    hsize_t maxsize_dim[] = {H5S_UNLIMITED};

    // Create a dataspace of rank 1, starting with 0 entries as above, growing
    // to unlimited entries.
    H5::DataSpace dataspace(1, starting_dim, maxsize_dim);
    LARCV_DEBUG() << "EventID Dataspace Created for new file" << std::endl;
    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList cparams;
    hsize_t chunk_dims[1] = {EVENT_ID_CHUNK_SIZE};
    cparams.setChunk(1, chunk_dims);
    cparams.setDeflate(EVENT_ID_COMPRESSION);

    _out_event_id_ds = _out_file.createDataSet(
        "Events/event_id", larcv3::EventID::get_datatype(), dataspace, cparams);

  }

  if (_io_mode != kWRITE) {
    prepare_input();
    if (!_in_entries_total) {
      LARCV_ERROR() << "Found 0 entries from input files..." << std::endl;
      return false;
    }
    LARCV_NORMAL() << "Prepared input with " << _in_entries_total
                   << " entries..." << std::endl;
    _read_id_bool.clear();
    _read_id_bool.resize(_product_ctr, true);
  }

  // Now handle "store-only" configuration
  // This has not been verified for larcv3
  _store_id_bool.clear();
  if (_io_mode != kREAD && _store_only.size()) {
    std::vector<size_t> store_only_id;
    for (auto const& type_name_s: _store_only) {
      auto const& type = type_name_s.first;
      auto const& name_s = type_name_s.second;
      for (auto const& name : name_s)
        store_only_id.push_back(register_producer(ProducerName_t(type,
        name)));
    }
    _store_id_bool.resize(_product_ctr, false);
    if ( _product_ctr > _read_id_bool.size() ) // append to read-in counters
      _read_id_bool.resize(_product_ctr, false);
    for (auto const& id : store_only_id) _store_id_bool.at(id) = true;
  }

  // _in_index = 0;
  _out_index = 0;
  _prepared = true;
  __ioman_mtx.unlock();

  return true;
}

size_t IOManager::register_producer(const ProducerName_t& name) {
  LARCV_DEBUG() << "start" << std::endl;

  std::string group_name = name.first + "_" + name.second + "_group";
  std::string group_loc = "/Data/" + group_name;

  LARCV_INFO() << "Requested to register a producer: " << name.second
               << " (Group " << group_name << ")" << std::endl;

  auto in_iter = _key_list.find(name);

  if (in_iter != _key_list.end()) {
    LARCV_INFO() << "... already registered. Returning a registered key "
                 << (*in_iter).second << std::endl;
    return (*in_iter).second;
  }

  _product_ptr_v[_product_ctr] =
      (EventBase*)(DataProductFactory::get().create(name));
  _product_type_v[_product_ctr] = name.first;
  _producer_name_v[_product_ctr] = name.second;

  // Determine the status of this product.  Check if it is in the input file:
  auto in_input_iter = _in_key_list.find(name);

  // If it's found, the status is kInputFileUnread
  if (in_input_iter != _in_key_list.end()){
    _product_status_v[_product_ctr] = kInputFileUnread;
  }
  else{
    // if it's not in the input, it's either virtual or output only.
    // It's virtual if the mode is kREAD
    // It's virtual if the mode is kWRITE/kBOTH AND this pair is not in the write list.
    if (_io_mode != kREAD){

      if (!_store_id_bool.empty() && _store_id_bool[_product_ctr]){
        _product_status_v[_product_ctr] = kOutputOnly;
      }
      else{
        _product_status_v[_product_ctr] = kVirtual;
      }
    }
    else {
      _product_status_v[_product_ctr] = kVirtual;
    }

  }


  const ProducerID_t id = _product_ctr;
  _key_list.insert(std::make_pair(name, id));

  _product_ctr += 1;

  LARCV_INFO() << "It is a new producer registration (key=" << id << ")"
               << std::endl;


  if (_io_mode != kREAD) {
    LARCV_INFO() << "kWRITE/kBOTH mode: creating an output group" << std::endl;
    LARCV_INFO() << "Data pointer: " << _product_ptr_v[id] << "(" << id << "/"
                  << _product_ptr_v.size() << ")" << std::endl;
    if (_out_group_v.size() <= id){
      _out_group_v.resize(id + 1);
    }
    _out_group_v[id] = _out_file.createGroup(group_loc.c_str());
    _product_ptr_v[id]->initialize(&_out_group_v[id]);
    LARCV_DEBUG() << "Created Group " << group_loc << " @ " << &_out_group_v[id] << std::endl;
  }else{

  }

  return id;
}

void IOManager::prepare_input() {
  logger::default_level(msg::kDEBUG);


  LARCV_DEBUG() << "start" << std::endl;
  if (_product_ctr) {
    LARCV_CRITICAL() << "Cannot call prepare_input before calling reset()!"
                     << std::endl;
    throw larbys();
  }

  // This function serves several purposes:
  // - It opens each file which ensures it exists
  // - It reads the eventID table and determines how many entries are in the
  // file
  // - It looks at subgroups in the "Data" group and uses that to determine
  //   which products are available
  //    - Subgroups are read for all files, and forced to be the same across all
  //    input files.

  // The following variables are populated:
  // _in_entries_v - number of entries per file
  // _in_index - set to 0 initially

  // Input Parameters
  // Current index in the input files - set it to 0 to start
  _in_index = 0;
  // Total number of input entries - updated for each file;
  _in_entries_total = 0;
  // Index of currently active file - start at file 0
  _in_active_file_index = 0;
  // Currently open file:
  // H5::H5File  _in_open_file;
  // the file is opened at the end of this function

  // List of total entries in input files?
  _in_entries_v.reserve(_in_file_v.size());

  // List of producer/product pairs in the input files
  _in_key_list.clear();

  LARCV_INFO() << "Start inspecting " << _in_file_v.size() << "files"
               << std::endl;
  for (size_t i_file = 0; i_file < _in_file_v.size(); ++i_file) {
    auto const& fname = _in_file_v[i_file];
    // auto const& dname = _in_dir_v[i_file];

    // H5::H5File* fin;

    LARCV_NORMAL() << "Opening a file in READ mode: " << fname << std::endl;
    open_new_input_file(fname);
    read_current_event_id();

    // Each file has (or should have) two groups: "Data" and "Events"

    try {
      H5::Group data = _in_open_file.openGroup("/Data");
      H5::Group events = _in_open_file.openGroup("/Events");
    } catch (...) {
      LARCV_CRITICAL() << "File " << fname
                       << " does not appear to be a larcv3 file, exiting."
                       << std::endl;
      throw larbys();
    }

    // Re-open those groups after the check
    H5::Group data = _in_open_file.openGroup("/Data");
    H5::Group events = _in_open_file.openGroup("/Events");

    // Vist the extents group and determine how many events are present:

    H5::DataSet extents = events.openDataSet("event_id");
    // Number of objects:
    hsize_t dims_current[1];
    extents.getSpace().getSimpleExtentDims(dims_current, NULL);
    // Number of entries is:

    LARCV_INFO() << "File " << fname << " has " << dims_current[0] << " entries"
                 << std::endl;

    // Append the number of events:
    _in_entries_v.push_back(dims_current[0]);
    _in_entries_total += dims_current[0];

    
    // Next, visit the available groups and see what producers are available.
    std::set<std::string> processed_object;
    for (size_t i_obj = 0; i_obj < data.getNumObjs(); ++i_obj) {
      char temp_name[128];
      // std::string obj_name = 
      int real_size = data.getObjnameByIdx(i_obj, temp_name, 128);
      std::string obj_name(temp_name);
      processed_object.insert(obj_name);
      char c[2] = "_";
      if (obj_name.find_first_of(c) > obj_name.size() ||
          obj_name.find_first_of(c) == obj_name.find_last_of(c)) {
        LARCV_CRITICAL() << "Skipping " << obj_name << " ... (not LArCV3 Group)"
                         << std::endl;
        continue;
      }

      std::string type_name(obj_name.substr(0, obj_name.find_first_of(c)));
      std::string suffix(
          obj_name.substr(obj_name.find_last_of(c) + 1,
                          obj_name.size() - obj_name.find_last_of(c)));
      std::string producer_name(obj_name.substr(
          obj_name.find_first_of(c) + 1,
          obj_name.find_last_of(c) - obj_name.find_first_of(c) - 1));

      if (suffix != "group") {
        LARCV_CRITICAL() << "Skipping " << obj_name << " ... (not LArCV3 Group)"
                         << std::endl;
        throw larbys();
        // continue;
      }

      if (_read_only.size()) {
        bool skip = true;
        auto const& type_name_iter = _read_only.find(type_name);
        if (type_name_iter != _read_only.end()) {
          auto const& type_name_s = (*type_name_iter).second;
          auto const& name_iter = type_name_s.find(producer_name);
          if (name_iter != type_name_s.end()) skip = false;
        }
        if (skip) {
          LARCV_NORMAL() << "Skipping: producer=" << producer_name
                         << " type= " << type_name << std::endl;
          continue;
        }
        LARCV_INFO() << "Not skipping: producer=" << producer_name
                     << " type= " << type_name << std::endl;
      }

      ProducerName_t name(type_name, producer_name);
      // If this is the first file, attempt to register the producer:
      if (i_file == 0) {
        auto id = register_producer(name);
        LARCV_INFO() << "Registered: producer=" << producer_name
                     << " Key=" << id << std::endl;
        // Set the product status:
        _product_status_v[id] = kInputFileUnread;
        _in_key_list.insert(std::make_pair(name, id));

      }


    }
    // After looping over all the objects, make sure there are none missing.
    // We've made sure every one found is supposed to be there, so it suffices
    // to make sure we have the same amount:
    if (processed_object.size() != _key_list.size()) {
      LARCV_CRITICAL() << "Group number mismatch across files!" << std::endl;
      throw larbys();
    }
  }

  // Make sure the first file is open:
  open_new_input_file(_in_file_v[0]);


}

void IOManager::open_new_input_file(std::string filename){
  
  H5::FileAccPropList  fapl(H5::FileAccPropList::DEFAULT);

  if (_h5_core_driver) {
    LARCV_INFO() << "File will be stored entirely on memory." << std::endl;
    // 1024 is number of bytes to increment each time more memory is needed; 
    //'false': do not write contents to disk when the file is closed
    fapl.setCore(1024, false);
  } 
  try{
    _in_open_file = H5::H5File(filename.c_str(), H5F_ACC_RDONLY, H5::FileCreatPropList::DEFAULT, fapl);
  }
  catch ( ... ) {
    LARCV_CRITICAL() << "Open attempt failed for a file: " << filename
                     << std::endl;
    throw larbys();
  }

  _active_in_event_id_dataset   = _in_open_file.openGroup("Events").openDataSet("event_id");
  _active_in_event_id_dataspace = _active_in_event_id_dataset.getSpace();

}

bool IOManager::read_entry(const size_t index, bool force_reload) {
  
  __ioman_mtx.lock();

  // Don't reopen groups unless absolutely necessary:
  _force_reopen_groups = false;

  LARCV_DEBUG() << "start" << std::endl;
  if (_io_mode == kWRITE) {
    LARCV_WARNING() << "Nothing to read in kWRITE mode..." << std::endl;
    return false;
  }
  if (!_prepared) {
    LARCV_CRITICAL() << "Cannot be called before initialize()!" << std::endl;
    throw larbys();
  }
  if (index >= _in_entries_total) {
    LARCV_ERROR() << "Input only has " << _in_entries_total << " entries!"
                  << std::endl;
    return false;
  }
  if (_in_index != index || force_reload) {
    _in_index = index;
    _event_id.clear();
    _set_event_id.clear();
    // read the entry from the event_id tree

    // First, what file is this?
    bool found = false;
    _current_offset = 0;
    size_t _this_file_index = kINVALID_SIZE;
    for (size_t i = 0; i < _in_entries_v.size(); ++i) {
      if (_in_index < _in_entries_v.at(i) + _current_offset) {
        found = true;
        _this_file_index = i;
        break;
      } else {
        _current_offset += _in_entries_v.at(i);
      }
    }
    if (!found || _this_file_index == kINVALID_SIZE) {
      LARCV_CRITICAL() << "Could not locate entry " << _in_index << std::endl;
      throw larbys();
    }

    if (_this_file_index != _in_active_file_index) {
      // Open a new file for reading:
      _in_active_file_index = _this_file_index;
      _in_open_file =
          H5::H5File(_in_file_v[_in_active_file_index].c_str(), H5F_ACC_RDONLY);

      _active_in_event_id_dataset   = _in_open_file.openGroup("Events").openDataSet("event_id");
      _active_in_event_id_dataspace = _active_in_event_id_dataset.getSpace();


      LARCV_INFO() << "Opening new file for continued event reading"
                     << std::endl;
      _force_reopen_groups = true;
    }


    read_current_event_id();

    // Make sure to reset all the data status:
    for (size_t id = 0; id < _product_status_v.size(); ++id){
    if (!_product_ptr_v[id]) break;
      if (_product_status_v[id] == kInputFileRead){
        _product_status_v[id] = kInputFileUnread;
      }
      else{
        // Do nothing?
      }
    }

    // // Now, we can open the events folder and figure out what's what.
    // // H5::DataSet events_dataset =
    //     // _in_open_file.openGroup("Events").openDataSet("event_id");
    // // H5::DataSpace events_dataspace = events_dataset.getSpace();

    // hsize_t events_slab_dims[1];
    // events_slab_dims[0] = 1;

    // hsize_t events_offset[1];
    // // Calculate the _current_offset based on requested index + _current_offset
    // // for this file
    // events_offset[0] = _in_index - _current_offset;

    // _active_in_event_id_dataspace.selectHyperslab(H5S_SELECT_SET, events_slab_dims,
    //                                  events_offset);

    // // Define memory space:
    // H5::DataSpace events_memspace(1, events_slab_dims);

    // EventID input_event_id;
    // // Write the new data
    // _active_in_event_id_dataset.read(&(input_event_id), _event_id_datatype,
    //                     events_memspace, _active_in_event_id_dataspace);
    // std::cout << "Active in event id for index " << index << ": " << input_event_id.event_key() << std::endl;
    // _event_id = input_event_id;

    // _in_open_file
    // Open the right file, if necessary
  }
  LARCV_DEBUG() << "Current input group index: " << _in_index << std::endl;

  __ioman_mtx.unlock();

  return true;
}

void IOManager::read_current_event_id(){
      // Now, we can open the events folder and figure out what's what.
    // H5::DataSet events_dataset =
        // _in_open_file.openGroup("Events").openDataSet("event_id");
    // H5::DataSpace events_dataspace = events_dataset.getSpace();

    hsize_t events_slab_dims[1];
    events_slab_dims[0] = 1;

    hsize_t events_offset[1];
    // Calculate the _current_offset based on requested index + _current_offset
    // for this file
    events_offset[0] = _in_index - _current_offset;

    _active_in_event_id_dataspace.selectHyperslab(H5S_SELECT_SET, events_slab_dims,
                                     events_offset);

    // Define memory space:
    H5::DataSpace events_memspace(1, events_slab_dims);

    EventID input_event_id;
    // Write the new data
    _active_in_event_id_dataset.read(&(input_event_id), _event_id_datatype,
                        events_memspace, _active_in_event_id_dataspace);
    _event_id = input_event_id;
}

bool IOManager::save_entry() {

  LARCV_DEBUG() << "start" << std::endl;
  if (!_prepared) {
    LARCV_CRITICAL() << "Cannot be called before initialize()!" << std::endl;
    throw larbys();
  }

  if (_io_mode == kREAD) {
    LARCV_ERROR() << "Cannot save in READ mode..." << std::endl;
    return false;
  }

  // in kBOTH mode make sure all Group entries are read-in
  if (_io_mode == kBOTH) {
    for (size_t id = 0; id < _out_group_v.size(); ++id) {
      if ( _store_id_bool.size() && 
           (id >= _store_id_bool.size() || !_store_id_bool[id]) 
          ) continue; 
      if (_product_status_v[id] == kInputFileUnread){
        get_data(id);
        _product_status_v[id] = kInputFileRead;
      }
      // if (_product_status_v[id] == kInputFileUnread)
      // std::cout << _out_read_entries[id] << std::endl;
      // if (!_out_read_entries[id]){
      //   std::cout << "Call get_data for id " << id << std::endl;
      // }
    }
  }

  LARCV_INFO() << "Saving new entry " << std::endl;


  set_id();

  if (_store_id_bool.empty()) {
    for (auto& p : _product_ptr_v) {
      if (!p) break;

    }

    // First, update the eventID group
    this->append_event_id();

    for (size_t i = 0; i < _out_group_v.size(); ++i) {
      auto t = &_out_group_v[i];
      auto& p = _product_ptr_v[i];

      if (!t) break;

      p->serialize(t);
      p->clear();
    }

  } else {
    for (size_t i = 0; i < _store_id_bool.size(); ++i) {
      auto const& p = _product_ptr_v[i];
      if (!_store_id_bool[i]) {
        p->clear();
        continue;
      }

    }

    for (size_t i = 0; i < _store_id_bool.size(); ++i) {
      if (!_store_id_bool[i]) continue;
      auto t = &_out_group_v[i];
      auto& p = _product_ptr_v[i];
      LARCV_DEBUG() << "Saving "
                    << t->fromClass()
                    // << " entry " << t->GetEntries()
                    << std::endl;
      p->serialize(t);
      p->clear();
    }
  }

  clear_entry();

  _out_entries += 1;
  _out_index += 1;

  return true;
}

void IOManager::append_event_id() {
  //////////////////////////////////////////////////////////////////
  // First, we get information about the current status of the dataset:
  //////////////////////////////////////////////////////////////////
  // Get the dataspace of the event ID dataset:
  H5::DataSpace dataspace = _out_event_id_ds.getSpace();

  // Get the dataset current size
  // The eventID table is exclusively rank-1
  hsize_t dims_current[1];
  // hsize_t * dims_current = new hsize_t[rank];
  dataspace.getSimpleExtentDims(dims_current, NULL);

  // Create a dimension for the data to add (which is the hyperslab data)
  hsize_t dims_of_slab[1];
  // For EventID, only adding one object per event
  dims_of_slab[0] = 1;

  //////////////////////////////////////////////////////////////////
  // Now we need to extend the dataset to accomodate the new dimensions
  //////////////////////////////////////////////////////////////////

  // Create a size vector for the FULL dataset: previous + current
  hsize_t size[1];
  size[0] = dims_current[0] + dims_of_slab[0];

  // Extend the dataset to accomodate the new data
  _out_event_id_ds.extend(size);

  // Now, select as a hyperslab the last section of data for writing:
  dataspace = _out_event_id_ds.getSpace();
  dataspace.selectHyperslab(H5S_SELECT_SET, dims_of_slab, dims_current);

  // Define memory space:
  H5::DataSpace memspace(1, dims_of_slab);

  // Write the new data
  _out_event_id_ds.write(&_event_id, EventID::get_datatype(), memspace,
                         dataspace);
}

void IOManager::clear_entry() {
  for (size_t id = 0; id < _product_status_v.size(); ++id){
    if (!_product_ptr_v[id]) break;
    if (_product_status_v[id] == kInputFileRead){
      _product_status_v[id] = kInputFileUnread;
    }
    else{
      // Do nothing?
    }
  }

  for (auto& p : _product_ptr_v) {
    if (!p) break;
    p->clear();
  }
  if (_set_event_id.valid()) {
    LARCV_DEBUG() << "Set _last_event_id to externally set values:"
                  << " run = " << _set_event_id.run()
                  << " subrun = " << _set_event_id.subrun()
                  << " event = " << _set_event_id.event() << std::endl;
    _last_event_id = _set_event_id;
  } else {
    LARCV_DEBUG() << "Set _last_event_id to inherited values:"
                  << " run = " << _event_id.run()
                  << " subrun = " << _event_id.subrun()
                  << " event = " << _event_id.event() << std::endl;
    _last_event_id = _event_id;
  }
  _event_id.clear();
  _set_event_id.clear();
}

ProducerID_t IOManager::producer_id(const ProducerName_t& name) const {
  LARCV_DEBUG() << "start" << std::endl;

  if (name.second.empty()) {
    LARCV_CRITICAL() << "Empty producer name (invalid)" << std::endl;
    throw larbys();
  }
  if (name.first.empty()) {
    LARCV_CRITICAL() << "Empty producer type (invalid)" << std::endl;
    throw larbys();
  }

  auto iter = _key_list.find(name);
  if (iter == _key_list.end()) {
    return kINVALID_PRODUCER;
  }
  return (*iter).second;
}

EventBase* IOManager::get_data(const std::string& type,
                               const std::string& producer) {
  LARCV_DEBUG() << "start" << std::endl;

  auto prod_name = ProducerName_t(type, producer);
  auto id = producer_id(prod_name);

  if (id == kINVALID_SIZE) {
    id = register_producer(prod_name);
    if (_io_mode == kREAD) {
      LARCV_NORMAL() << type << " created w/ producer name " << producer
                     << " but won't be stored in file (kREAD mode)"
                     << std::endl;
      _product_status_v[id] = kVirtual;
    }
    // We specify that get data has already been "called" on this producer/product
    // to make it virtual until serialization, if it ever happens.

  }
  return get_data(id);
}

EventBase* IOManager::get_data(const size_t id) {
  __ioman_mtx.lock();

  LARCV_DEBUG() << "start" << std::endl;

  if (id >= _product_ctr) {
    LARCV_ERROR() << "Invalid producer ID requested:" << id << std::endl;
    throw larbys();
  }

  if (_io_mode != kWRITE && _in_index != kINVALID_SIZE &&
      (id >= _read_id_bool.size() || _read_id_bool[id]) &&
      _product_status_v[id] == kInputFileUnread ) {
    // Reading in is just getting the group, calling deserialize with the right
    // index.

    // The group name is "product_producer_group"
    std::string group_name = _product_type_v[id];
    group_name = "Data/" + group_name + "_" + _producer_name_v[id] + "_group";

    // std::cout << "_in_index: " << _in_index << std::endl;
    // std::cout << "_current_offset: " << _current_offset << std::endl;

    H5::Group group;
    auto iter = _groups.find(group_name);
    if (iter == _groups.end() || _force_reopen_groups) {
      group = _in_open_file.openGroup(group_name.c_str());
      _groups[group_name] = group;
    } else {
      group = iter->second;
    }

    try {
      _product_ptr_v[id]->deserialize(&group, _in_index - _current_offset, _force_reopen_groups);
      _product_status_v[id] = kInputFileRead;
    }
    catch (...){
      // When there is an error in deserialization, close the open input file gracefully:
      if(_io_mode != kWRITE){
        LARCV_CRITICAL() << "Exception caught in deserialization, closing input file gracefully" << std::endl;
        _in_open_file.close();
      }
    }
  }
  __ioman_mtx.unlock();
  return _product_ptr_v[id];
}

void IOManager::set_id(const long run, const long subrun,
                       const long event) {
  if (_io_mode == kREAD) {
    LARCV_CRITICAL() << "Cannot change event id in kREAD mode" << std::endl;
    throw larbys();
  }

  EventID tmp;
  tmp.run(run);
  tmp.subrun(subrun);
  tmp.event(event);

  LARCV_INFO() << "Request to set event id: " << tmp.event_key() << std::endl;

  if (_set_event_id.valid() && _set_event_id != tmp)
    LARCV_INFO() << "Force setting (run,subrun,event) ID as (" << run << ","
                 << subrun << "," << event << ")" << std::endl;

  _set_event_id = tmp;
}

void IOManager::set_id() {
  LARCV_DEBUG() << "start" << std::endl;

  if (_io_mode == kREAD) {
    return;
  }

  if (_set_event_id.valid()) {
    _event_id = _set_event_id;
  }

  LARCV_INFO() << "Setting event id for output groups: "
               << _event_id.event_key() << std::endl;

  // for (size_t i = 0; i < _product_ptr_v.size(); ++i) {
  //   auto& p = _product_ptr_v[i];
  //   if (!p) break;
  // }
}

void IOManager::finalize() {
  LARCV_DEBUG() << "start" << std::endl;
  if (_io_mode != kREAD) {
    // _out_file->cd();
    // if (_store_id_bool.empty()) {
    //   for (auto& t : _out_group_v) {
    //     // if (!t) break;
    //     LARCV_NORMAL() << "Writing "
    //                    << t.fromClass()
    //                    << std::endl;
    //   }
    // } else {
    //   for (size_t i = 0; i < _store_id_bool.size(); ++i) {
    //     if (!_store_id_bool[i]) continue;
    //     auto t = &_out_group_v[i];
    //     LARCV_NORMAL() << "Writing "
    //                    << t->fromClass()
    //                    << std::endl;
    //   }
    // }
    LARCV_NORMAL() << "Closing output file" << std::endl;
    _out_file.close();
  }

  LARCV_INFO() << "Deleting data pointers" << std::endl;
  for (auto& p : _product_ptr_v) {
    delete p;
  }

  reset();
}

void IOManager::reset() {
  LARCV_DEBUG() << "start" << std::endl;
  _event_id.clear();
  _set_event_id.clear();
  _in_entries_v.clear();
  _out_group_v.clear();
  // _out_group_v.resize(1000, nullptr);
  _product_ptr_v.clear();
  _product_ptr_v.resize(1000, nullptr);
  _product_type_v.clear();
  _product_type_v.resize(1000, "");
  _producer_name_v.clear();
  _producer_name_v.resize(1000, "");
  _product_status_v.clear();
  _product_status_v.resize(1000, kUnknown);
  _product_ctr = 0;
  _in_index = 0;
  _current_offset = 0;
  _out_index = 0;
  _in_entries_total = 0;
  _prepared = false;
  _out_file_name = "";
  _in_file_v.clear();
  _in_dir_v.clear();
  _key_list.clear();
  _in_key_list.clear();
  _read_only.clear();
  _store_only.clear();
  _read_id_bool.clear();
  _store_id_bool.clear();
}

}  // namespace larcv3
#endif
