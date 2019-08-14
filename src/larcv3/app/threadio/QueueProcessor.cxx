#ifndef __LARCV3THREADIO_QueuePROCESSOR_CXX
#define __LARCV3THREADIO_QueuePROCESSOR_CXX

#include "QueueProcessor.h"
#include "larcv3/core/base/LArCVBaseUtilFunc.h"
#include <sstream>
#include <unistd.h>
#include "BatchFillerTemplate.h"
#include "BatchDataQueueFactory.h"
#include <mutex>
#include <chrono>

#ifdef LARCV_OPENMP
#include <omp.h>
#endif

namespace larcv3 {
int omp_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    n += 1;
    return n;
}

  
  QueueProcessor::QueueProcessor(std::string name)
    : larcv_base(name)
    , _processing(false)
    , _configured(false)
    , _batch_global_counter(0)
  {}

  QueueProcessor::~QueueProcessor()
  { reset(); }

  const std::string& QueueProcessor::storage_name(size_t process_id) const
  {
    if (process_id > _process_name_v.size()) {
      LARCV_CRITICAL() << "Process ID " << process_id << " is invalid!" << std::endl;
      throw larbys();
    }
    return _process_name_v[process_id];
  }

  size_t QueueProcessor::process_id(const std::string& name) const
  {
    for (size_t id = 0; id < _process_name_v.size(); ++id)
      if (name == _process_name_v[id]) return id;
    LARCV_ERROR() << "Could not locate process name: " << name << std::endl;
    return kINVALID_SIZE;
  }


  const std::vector<size_t>& QueueProcessor::processed_entries() const
  {
    return _current_batch_entries_v;
  }

  const std::vector<larcv3::EventID>& QueueProcessor::processed_events() const
  {
    return _current_batch_events_v;
  }

  size_t QueueProcessor::get_n_entries() const
  {
    return _driver.io().get_n_entries();
  }

  const ProcessDriver & QueueProcessor::pd() const
  {
    return _driver;
  }

  void QueueProcessor::set_next_index(size_t index)
  {
    _next_index_v.resize(1);
    _next_index_v[0] = index;
  }


  void QueueProcessor::set_next_batch(const std::vector<size_t>& index_v)
  {
    _next_index_v = index_v;
  }

  void QueueProcessor::reset()
  {
    // per-thread variables
    _driver.reset();

    _current_batch_entries_v.clear();
    _current_batch_events_v.clear();
    _next_batch_entries_v.clear();
    _next_batch_events_v.clear();

    // per-process variables
    _process_name_v.clear();


    _next_index_v.clear();
    
    // others
    _configured = false;
    _batch_global_counter = 0;
  }

  void QueueProcessor::configure(const std::string config_file, int color)
  {
    LARCV_DEBUG() << "Called" << std::endl;
    // check state
    if (_processing) {
      LARCV_CRITICAL() << "Must call finalize() before calling initialize() after starting to process..." << std::endl;
      throw larbys();
    }
    // check cfg file
    if (config_file.empty()) {
      LARCV_CRITICAL() << "Config file not set!" << std::endl;
      throw larbys();
    }

    // check cfg content top level
    auto main_cfg = CreatePSetFromFile(config_file);
    if (!main_cfg.contains_pset(name())) {
      LARCV_CRITICAL() << "QueueProcessor configuration (" << name() << ") not found in the config file (dump below)" << std::endl
                       << main_cfg.dump()
                       << std::endl;
      throw larbys();
    }
    auto const cfg = main_cfg.get<larcv3::PSet>(name());
    configure(cfg);
  }

  void QueueProcessor::pop_current_data()
  {

    bool ready = true;

    for (size_t pid = 0; pid < _process_name_v.size(); ++pid) {
      auto const& name = _process_name_v[pid];
      auto const& datatype = _batch_data_type_v[pid];
      switch ( datatype ) {
      case BatchDataType_t::kBatchDataShort:
        ready = ready && BatchDataQueueFactory<short>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataInt:
        ready = ready && BatchDataQueueFactory<int>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataFloat:
        ready = ready && BatchDataQueueFactory<float>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataDouble:
        ready = ready && BatchDataQueueFactory<double>::get().get_queue(name).is_next_ready(); break;
      default:
        LARCV_CRITICAL() << "Process name " << name
                         << " encountered none-supported BatchDataType_t: " << int(datatype) << std::endl;
        throw larbys();
      }
    }

    if (! ready){
      LARCV_ERROR() << "Can't pop current data because the next data is not yet ready." << std::endl;
    }

    for (size_t pid = 0; pid < _process_name_v.size(); ++pid) {
      auto const& name = _process_name_v[pid];
      auto const& datatype = _batch_data_type_v[pid];
      switch ( datatype ) {
      case BatchDataType_t::kBatchDataShort:
        BatchDataQueueFactory<short>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataInt:
        BatchDataQueueFactory<int>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataFloat:
        BatchDataQueueFactory<float>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataDouble:
        BatchDataQueueFactory<double>::get_writeable().get_queue_writeable(name).pop(); break;
      default:
        LARCV_CRITICAL() << "Process name " << name
                         << " encountered none-supported BatchDataType_t: " << int(datatype) << std::endl;
        throw larbys();
      }
    }

    // Calling pop_all on the batches will automatically promote next to current;
    // Therefore, promote the indexing too:
    _current_batch_events_v = std::move(_next_batch_events_v);
    _current_batch_entries_v = std::move(_next_batch_entries_v);
  }

  void QueueProcessor::configure(const PSet& orig_cfg, int color)
  {
    reset();

    
    // std::cout << "\033[93m setting verbosity \033[00m" << orig_cfg.get<unsigned short>("Verbosity", 2) << std::endl;
    set_verbosity( (msg::Level_t)(orig_cfg.get<unsigned short>("Verbosity", 2)) );

    _input_fname_v = orig_cfg.get<std::vector<std::string> >("InputFiles");

    _process_name_v.clear();

    // Initialize the ProcessDriver:


    std::stringstream ss_tmp1;
    ss_tmp1 << name();

    std::string proc_name(ss_tmp1.str());
    std::string io_cfg_name = "IOManager";

    LARCV_INFO() << "Constructing Processor config: " << proc_name << std::endl;
    PSet proc_cfg(proc_name);
    for (auto const& value_key : orig_cfg.value_keys()) {
      if (value_key == "ProcessName") {
        for (auto const& unit_name : orig_cfg.get<std::vector<std::string> >("ProcessName")) {
          _process_name_v.push_back(unit_name);
        }
      }
      if (value_key == "RandomAccess"){
        continue;
      }
      proc_cfg.add_value(value_key, orig_cfg.get<std::string>(value_key));
    }
    proc_cfg.add_value("RandomAccess", "false");

    // Brew read-only configuration
    PSet io_cfg(io_cfg_name);
    io_cfg.add_value("Verbosity", std::string(std::to_string(logger().level())));
    io_cfg.add_value("Name", io_cfg_name);
    io_cfg.add_value("IOMode", "0");
    io_cfg.add_value("OutFileName", "");
    io_cfg.add_value("StoreOnlyType", "[]");
    io_cfg.add_value("StoreOnlyName", "[]");

    LARCV_INFO() << "Constructing IO configuration: " << io_cfg_name << std::endl;

    for (auto const& pset_key : orig_cfg.pset_keys()) {
      if (pset_key == "IOManager") {
        auto const& orig_io_cfg = orig_cfg.get_pset(pset_key);
        LARCV_NORMAL() << "IOManager configuration will be ignored..." << std::endl;
      }
      else if (pset_key == "ProcessList") {
        auto const& orig_thread_plist = orig_cfg.get<larcv3::PSet>(pset_key);
        proc_cfg.add_pset(orig_thread_plist);
      }
      else
        proc_cfg.add_pset(orig_cfg.get_pset(pset_key));



      proc_cfg.add_pset(io_cfg);


      LARCV_INFO() << "Enforcing configuration ..." << std::endl;

      LARCV_INFO() << proc_cfg.dump() << std::endl;

      // configure the driver
      _driver.reset();
      _driver.configure(proc_cfg);
      _driver.override_input_file(_input_fname_v);


      LARCV_NORMAL() << "Done configuration ..." << std::endl;

      // Check & report batch filler's presence
      for (auto const& process_name : _driver.process_names()) {
        ProcessID_t id = _driver.process_id(process_name);

        auto ptr = _driver.process_ptr(id);

        LARCV_INFO() << "Process " << process_name << " ... ID=" << id << "... BatchFiller? : " << ptr->is("BatchFiller") << std::endl;
      }
      _driver.initialize(color);

      // only-once-operation among all queueus: initialize storage
      _batch_filler_id_v.clear();
      _batch_data_type_v.clear();
      for (size_t pid = 0; pid < _process_name_v.size(); ++pid) {
        auto proc_ptr = _driver.process_ptr(pid);
        if (!(proc_ptr->is("BatchFiller"))) continue;
        _batch_filler_id_v.push_back(pid);
        _batch_data_type_v.push_back( ((BatchHolder*)(proc_ptr))->data_type() );
        auto const& name = _process_name_v[pid];
        switch ( _batch_data_type_v.back() ) {
        case BatchDataType_t::kBatchDataShort:
          BatchDataQueueFactory<short>::get_writeable().make_queue(name); break;
        case BatchDataType_t::kBatchDataInt:
          BatchDataQueueFactory<int>::get_writeable().make_queue(name); break;
        case BatchDataType_t::kBatchDataFloat:
          BatchDataQueueFactory<float>::get_writeable().make_queue(name); break;
        case BatchDataType_t::kBatchDataDouble:
          BatchDataQueueFactory<double>::get_writeable().make_queue(name); break;
        default:
          LARCV_CRITICAL() << "Process name " << name
                           << " encountered none-supported BatchDataType_t: " << (int)(((BatchHolder*)(proc_ptr))->data_type()) << std::endl;
          throw larbys();
        }
      }
    }
    _configured = true;
  }

  
  bool QueueProcessor::batch_process() {

    LARCV_DEBUG() << " start" << std::endl;

    // must be configured
    if (!_configured) {
      LARCV_ERROR() << "Must call configure() before run process!" << std::endl;
      return false;
    }
    // must be non-zero entries to process
    if (!_next_index_v.size()) {
      LARCV_ERROR() << "_next_index_v.size() must be positive integer..." << std::endl;
      return false;
    }

    //
    // execute
    //
    _processing = true;

    set_batch_storage();

    begin_batch();


    _next_batch_entries_v.clear();
    _next_batch_entries_v.resize(_next_index_v.size());
    _next_batch_events_v.clear();
    _next_batch_events_v.resize(_next_index_v.size());

    // LARCV_INFO() << "Entering process loop" << std::endl;
    // auto start = std::chrono::steady_clock::now();

#ifdef LARCV_OPENMP
    std::cout << "Number of threads: " << omp_thread_count() << std::endl;
#endif
    size_t i(0), i_entry(0);

    // #pragma omp parallel 
    // {
    //   #pragma omp single
    //   {
        for(i_entry =0; i_entry < _next_index_v.size(); ++ i_entry){
          // #pragma omp task
          // {
            auto & entry = _next_index_v[i_entry];
            LARCV_INFO() << "Processing entry: " << entry << std::endl;

            bool good_status = _driver.process_entry(entry, true);
            LARCV_INFO() << "Finished processing event id: " << _driver.event_id().event_key() << std::endl;
            _next_batch_entries_v.at(i_entry) = entry;
            _next_batch_events_v.at(i_entry) = _driver.event_id();
          // }
        }
    //   }
    // }
    

    // auto duration = std::chrono::duration_cast< std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);


    // std::cout << "Duration of omp loop: " << duration.count() << std::endl;
    // LARCV_DEBUG() << " end" << std::endl;

    end_batch();
    
    _processing = false;
    return true;

  }

  bool QueueProcessor::set_batch_storage(){

    for (size_t pid = 0; pid < _process_name_v.size(); ++pid) {
      auto proc_ptr = _driver.process_ptr(pid);
      if (!(proc_ptr->is("BatchFiller"))) continue;

      auto const& name = _process_name_v[pid];
      BatchDataState_t batch_state = BatchDataState_t::kBatchStateUnknown;
      switch ( ((BatchHolder*)(proc_ptr))->data_type() ) {
      case BatchDataType_t::kBatchDataShort:
        ((BatchFillerTemplate<short>*)proc_ptr)->_batch_data_ptr
          = &(BatchDataQueueFactory<short>::get_writeable().get_queue_writeable(name).get_next_writeable());
        batch_state = ((BatchFillerTemplate<short>*)proc_ptr)->_batch_data_ptr->state();
        break;
      case BatchDataType_t::kBatchDataInt:
        ((BatchFillerTemplate<int>*)proc_ptr)->_batch_data_ptr
          = &(BatchDataQueueFactory<int>::get_writeable().get_queue_writeable(name).get_next_writeable());
        batch_state = ((BatchFillerTemplate<int>*)proc_ptr)->_batch_data_ptr->state();
        break;
      case BatchDataType_t::kBatchDataFloat:
        ((BatchFillerTemplate<float>*)proc_ptr)->_batch_data_ptr
          = &(BatchDataQueueFactory<float>::get_writeable().get_queue_writeable(name).get_next_writeable());
        batch_state = ((BatchFillerTemplate<float>*)proc_ptr)->_batch_data_ptr->state();
        break;
      case BatchDataType_t::kBatchDataDouble:
        ((BatchFillerTemplate<double>*)proc_ptr)->_batch_data_ptr
          = &(BatchDataQueueFactory<double>::get_writeable().get_queue_writeable(name).get_next_writeable());
        batch_state = ((BatchFillerTemplate<double>*)proc_ptr)->_batch_data_ptr->state();
        break;
      default:
        LARCV_CRITICAL() << "Process name " << name
                         << " encountered none-supported BatchDataType_t: " << (int)(((BatchHolder*)(proc_ptr))->data_type()) << std::endl;
        throw larbys();
      }

      // check to make sure BatchData is ready to be filled
      if (batch_state != BatchDataState_t::kBatchStateEmpty &&
          batch_state != BatchDataState_t::kBatchStateUnknown &&
          batch_state != BatchDataState_t::kBatchStateFilled ) {
        LARCV_CRITICAL() << " cannot fill storage " 
                         << " because its state (" << (int)batch_state
                         << ") is neither kBatchStateUnknown nor kBatchStateEmpty nor kBatchStateFilled!" << std::endl;
        throw larbys();
      }
    }
    return true;
  }

  bool QueueProcessor::begin_batch(){
    LARCV_INFO() << "Executing batch_begin ... " << std::endl;
    for (auto const& process_name : _driver.process_names()) {
      LARCV_INFO() << process_name << std::endl;
      ProcessID_t id = _driver.process_id(process_name);
      auto ptr = _driver.process_ptr(id);
      if (!(ptr->is("BatchFiller"))) continue;
      LARCV_INFO() << "Executing " << process_name << "::batch_begin()" << std::endl;
      ((BatchHolder*)(ptr))->_batch_size = _next_index_v.size();
      switch ( ((BatchHolder*)(ptr))->data_type() ) {
      case BatchDataType_t::kBatchDataShort:
        ((BatchFillerTemplate<short>*)ptr)->batch_begin(); break;
      case BatchDataType_t::kBatchDataInt:
        ((BatchFillerTemplate<int>*)ptr)->batch_begin(); break;
      case BatchDataType_t::kBatchDataFloat:
        ((BatchFillerTemplate<float>*)ptr)->batch_begin(); break;
      case BatchDataType_t::kBatchDataDouble:
        ((BatchFillerTemplate<double>*)ptr)->batch_begin(); break;
      default:
        LARCV_CRITICAL() << " encountered none-supported BatchDataType_t: " << (int)(((BatchHolder*)(ptr))->data_type()) << std::endl;
        throw larbys();
      }
    }
    return true;
  }
  bool QueueProcessor::end_batch(){
    for (auto const& process_name : _driver.process_names()) {
      ProcessID_t id = _driver.process_id(process_name);
      auto ptr = _driver.process_ptr(id);
      if (!(ptr->is("BatchFiller"))) continue;

      switch ( ((BatchHolder*)(ptr))->data_type() ) {
      case BatchDataType_t::kBatchDataShort:
        ((BatchFillerTemplate<short>*)ptr)->batch_end(); break;
      case BatchDataType_t::kBatchDataInt:
        ((BatchFillerTemplate<int>*)ptr)->batch_end(); break;
      case BatchDataType_t::kBatchDataFloat:
        ((BatchFillerTemplate<float>*)ptr)->batch_end(); break;
      case BatchDataType_t::kBatchDataDouble:
        ((BatchFillerTemplate<double>*)ptr)->batch_end(); break;
      default:
        LARCV_CRITICAL() << " encountered none-supported BatchDataType_t: " << (int)(((BatchHolder*)(ptr))->data_type()) << std::endl;
        throw larbys();
      }
    }
    return true;
  }



}

#endif
