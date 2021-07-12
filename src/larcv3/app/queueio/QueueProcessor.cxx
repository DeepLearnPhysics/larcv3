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

#include "larcv3/core/dataformat/Particle.h"

#ifdef LARCV_OPENMP
#include <omp.h>
#endif

namespace larcv3 {
#ifdef LARCV_OPENMP
int omp_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    n += 1;
    return n;
}
#endif


  QueueProcessor::QueueProcessor(std::string name)
    : larcv_base(name)
    , _processing(false)
    , _configured(false)
    , _batch_global_counter(0)
  {

    std::future<bool> f = std::async(std::launch::deferred, [](){return true;});
    _preparation_future = std::move(f);
  }

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

  void QueueProcessor::set_next_batch(pybind11::array_t<size_t> index_v)
  {
    _next_index_v.clear();

    pybind11::buffer_info buf = index_v.request();

    _next_index_v.resize(buf.size);
    size_t * ptr = (size_t *) buf.ptr;
    for (size_t i = 0; i < buf.size; i++ ){
      _next_index_v[i] = ptr[i];
    }
  }

  bool QueueProcessor::is_reading() const {
    return !(_preparation_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
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

  void QueueProcessor::pop_current_data()
  {


    bool ready = true;

    for (size_t pid = 0; pid < _process_name_v.size(); ++pid) {
      auto const& name = _process_name_v[pid];

      auto proc_ptr = _driver.process_ptr(pid);
      if (!(proc_ptr->is("BatchFiller"))) continue;
      auto datatype = ((BatchHolder*)(proc_ptr))->data_type() ;


      switch ( datatype ) {
      case BatchDataType_t::kBatchDataShort:
        ready = ready && BatchDataQueueFactory<short>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataInt:
        ready = ready && BatchDataQueueFactory<int>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataFloat:
        ready = ready && BatchDataQueueFactory<float>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataDouble:
        ready = ready && BatchDataQueueFactory<double>::get().get_queue(name).is_next_ready(); break;
      case BatchDataType_t::kBatchDataParticle:
        ready = ready && BatchDataQueueFactory<larcv3::Particle>::get().get_queue(name).is_next_ready(); break;
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

      auto proc_ptr = _driver.process_ptr(pid);
      if (!(proc_ptr->is("BatchFiller"))) continue;
      // _batch_filler_id_v.push_back(pid);
      auto datatype = ((BatchHolder*)(proc_ptr))->data_type() ;

      switch ( datatype ) {
      case BatchDataType_t::kBatchDataShort:
        BatchDataQueueFactory<short>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataInt:
        BatchDataQueueFactory<int>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataFloat:
        BatchDataQueueFactory<float>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataDouble:
        BatchDataQueueFactory<double>::get_writeable().get_queue_writeable(name).pop(); break;
      case BatchDataType_t::kBatchDataParticle:
        BatchDataQueueFactory<larcv3::Particle>::get_writeable().get_queue_writeable(name).pop(); break;
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

  void QueueProcessor::configure(const json& orig_cfg, int color)
  {
    reset();
    config = this->default_config();
    larcv3::logger::default_level(msg::kDEBUG);

    // Get the default config

    // Add the user arguments
    config = augment_default_config(config, orig_cfg);


    set_verbosity( config["Verbosity"].get<msg::Level_t>() );

    auto _input_fname_v = config["InputFiles"].get<std::vector<std::string>>();


    _process_name_v.clear();


    // Initialize the ProcessDriver:

    // To init the process driver, we pull the default config (which includes an IOConfig)
    // and then update as needed.

    // Three important pieces here to fill in:

    // ProcessType
    // ProcessName
    // ProcessList

    // We have already the default config, so it's a matter of making sure we've got the rest.



    // LARCV_INFO() << "Constructing Processor config: " << proc_name << std::endl;
    // json proc_cfg;
    // for (auto const& value_key : orig_cfg.value_keys()) {
    //   if (value_key == "ProcessName") {
    //     for (auto const& unit_name : orig_cfg.get<std::vector<std::string> >("ProcessName")) {
    //       _process_name_v.push_back(unit_name);
    //     }
    //   }
    //   if (value_key == "RandomAccess"){
    //     continue;
    //   }
    //   proc_cfg.add_value(value_key, orig_cfg.get<std::string>(value_key));
    // }
    // proc_cfg.add_value("RandomAccess", "false");




    // LARCV_INFO() << "Constructing IO configuration: " << io_cfg_name << std::endl;

    // for (auto const& pset_key : orig_cfg.pset_keys()) {
    //   if (pset_key == "IOManager") {
    //     // auto const& orig_io_cfg = orig_cfg.get_pset(pset_key);
    //     LARCV_NORMAL() << "IOManager configuration will be ignored..." << std::endl;
    //   }
    //   else if (pset_key == "ProcessList") {
    //     auto const& orig_thread_plist = orig_cfg.get<larcv3::PSet>(pset_key);
    //     proc_cfg.add_pset(orig_thread_plist);
    //   }
    //   else
    //     proc_cfg.add_pset(orig_cfg.get_pset(pset_key));


    auto & proc_cfg = config["ProcessDriver"];


    LARCV_INFO() << "Enforcing configuration ..." << std::endl;


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
      _process_name_v.push_back(process_name);
    }
    _driver.initialize(color);

    // Prepare P}

    // only-once-operation among all queueus: initialize storage
    _batch_filler_id_v.clear();
    _batch_data_type_v.clear();
    for (size_t pid = 0; pid < _process_name_v.size(); ++pid) {


      auto proc_ptr = _driver.process_ptr(pid);
      if (!(proc_ptr->is("BatchFiller"))) continue;
      _batch_filler_id_v.push_back(pid);
      auto datatype = ((BatchHolder*)(proc_ptr))->data_type() ;
      _batch_data_type_v.push_back(datatype);
      auto const& name = _process_name_v[pid];
      switch ( datatype ) {
      case BatchDataType_t::kBatchDataShort:
        BatchDataQueueFactory<short>::get_writeable().make_queue(name); break;
      case BatchDataType_t::kBatchDataInt:
        BatchDataQueueFactory<int>::get_writeable().make_queue(name); break;
      case BatchDataType_t::kBatchDataFloat:
        BatchDataQueueFactory<float>::get_writeable().make_queue(name); break;
      case BatchDataType_t::kBatchDataDouble:
        BatchDataQueueFactory<double>::get_writeable().make_queue(name); break;
      case BatchDataType_t::kBatchDataParticle:
        BatchDataQueueFactory<larcv3::Particle>::get_writeable().make_queue(name); break;
      default:
        LARCV_CRITICAL() << "Process name " << name
                         << " encountered none-supported BatchDataType_t: " << (int)(((BatchHolder*)(proc_ptr))->data_type()) << std::endl;
        throw larbys();
      }
    }

    _configured = true;
  }


  void QueueProcessor::prepare_next() {

    _preparation_future.wait();



    _processing = true;
    std::future<bool> fut = std::async(std::launch::async,
                                       &QueueProcessor::batch_process, this);

    _preparation_future = std::move(fut);

    return;

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

    LARCV_INFO() << "Entering process loop" << std::endl;
    // auto start = std::chrono::steady_clock::now();

#ifdef LARCV_OPENMP
    std::cout << "Number of threads: " << omp_thread_count() << std::endl;
#endif
    // size_t i(0),
    size_t i_entry(0);

    // #pragma omp parallel
    // {
    //   #pragma omp single
    //   {
        for(i_entry =0; i_entry < _next_index_v.size(); ++ i_entry){
          // #pragma omp task
          // {
            auto & entry = _next_index_v[i_entry];
            LARCV_INFO() << "Processing entry: " << entry << std::endl;

            // bool good_status =
            _driver.process_entry(entry, true);
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

    // if (_preparation_thread.joinable()) {
    //   _preparation_thread.join();
    // }
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
      case BatchDataType_t::kBatchDataParticle:
        ((BatchFillerTemplate<larcv3::Particle>*)proc_ptr)->_batch_data_ptr
          = &(BatchDataQueueFactory<larcv3::Particle>::get_writeable().get_queue_writeable(name).get_next_writeable());
        batch_state = ((BatchFillerTemplate<larcv3::Particle>*)proc_ptr)->_batch_data_ptr->state();
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
      case BatchDataType_t::kBatchDataParticle:
        ((BatchFillerTemplate<larcv3::Particle>*)ptr)->batch_begin(); break;
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
      case BatchDataType_t::kBatchDataParticle:
        ((BatchFillerTemplate<larcv3::Particle>*)ptr)->batch_end(); break;
      default:
        LARCV_CRITICAL() << " encountered none-supported BatchDataType_t: " << (int)(((BatchHolder*)(ptr))->data_type()) << std::endl;
        throw larbys();
      }
    }
    return true;
  }



}

#include <pybind11/stl.h>

void init_queueprocessor(pybind11::module m){

  using Class = larcv3::QueueProcessor;
  pybind11::class_<Class> queueproc(m, "QueueProcessor");

  queueproc.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "QueueProcessor");

  queueproc.def("batch_process",                    &Class::batch_process);
  queueproc.def("prepare_next",                     &Class::prepare_next);
  queueproc.def("reset",                            &Class::reset);
  queueproc.def("configure",
    (void (Class::*)(const json&, int)) (           &Class::configure),
    pybind11::arg("cfg"),
    pybind11::arg("color")=0);
  queueproc.def("configured",                       &Class::configured);
  queueproc.def("pop_current_data",                 &Class::pop_current_data);
  queueproc.def("set_next_index",                   &Class::set_next_index);
  queueproc.def("set_next_batch",
    (void (Class::*)(const std::vector<size_t>&)) ( &Class::set_next_batch));

  queueproc.def("set_next_batch",
    (void (Class::*)(pybind11::array_t <size_t> )) (&Class::set_next_batch));

  queueproc.def("is_reading",                       &Class::is_reading);
  queueproc.def("get_n_entries",                    &Class::get_n_entries);
  queueproc.def("processed_entries",                &Class::processed_entries);
  queueproc.def("processed_events",                 &Class::processed_events);
  queueproc.def("pd",                               &Class::pd);
  queueproc.def("storage_name",                     &Class::storage_name);
  queueproc.def("process_id",                       &Class::process_id);
  queueproc.def("batch_fillers",                    &Class::batch_fillers);
  queueproc.def("batch_types",                      &Class::batch_types);
  queueproc.def("default_config",                   &Class::default_config);


}

#endif
