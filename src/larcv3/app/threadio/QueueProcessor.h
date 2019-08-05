/**
 * \file QueueProcessor.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class QueueProcessor
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_QUEUEPROCESSOR_H
#define __LARCV3THREADIO_QUEUEPROCESSOR_H

#include "larcv3/core/processor/ProcessDriver.h"
#include "ThreadIOTypes.h"
#include <random>

namespace larcv3 {
  /**
     \class QueueProcessor
     User defined class QueueProcessor ... these comments are used to generate
     doxygen docume ntation!
  */
  class QueueProcessor : public larcv_base {

  public:

    /// Default constructor
    QueueProcessor(std::string name = "QueueProcessor");

    /// Default destructor
    ~QueueProcessor();

    // Process a batch of entries, using _next_index_v to specify entries
    bool batch_process();



    // Reset the state
    void reset();

    // configure the processor from a file on disk
    void configure(const std::string config_file);

    // configure the processor from a PSet object    
    void configure(const PSet& cfg);

    // Check if the processor is configured
    inline bool configured() const { return _configured;}

    // void release_data(size_t storage_id);

    void pop_current_data();

    // void release_data();

    // inline const std::vector<larcv3::BatchDataState_t>& storage_status_array() const
    // { return _batch_state_v; }

    // inline size_t num_threads() const { return _num_threads; }

    // Set the next index to read/
    // This will just set the index_v object
    void set_next_index(size_t index);

    // Set the next set of indexes to read:
    void set_next_batch(const std::vector<size_t>& index_v);

    // / Sets the first entry the IOManager should look at 
    // void set_start_entry(size_t entry);

    // /// Sets the number of entries that should be skipped between batches 
    // void set_entries_skip(size_t nskip);

    // bool thread_running(size_t thread_id) const;

    // bool thread_running() const;

    bool is_reading() const {return _processing;}

    // size_t num_thread_running() const;

    void status_dump() const;

    // size_t process_counter(size_t thread_id) const;

    // size_t process_counter() const;

    // inline const std::vector<larcv3::ThreadFillerState_t>& thread_status() const
    // { return _thread_state_v; }

    // Get number of entries possible to read
    size_t get_n_entries() const;

        
    // size_t batch_id(size_t storage_id) const;

    // inline const std::vector<size_t>& thread_exec_counters() const
    // { return _thread_exec_ctr_v; }
    
    // Return the list of processed entries for current data
    const std::vector<size_t>& processed_entries() const;

    // Return the list of processed eventIDs for current data
    const std::vector<larcv3::EventID>& processed_events() const;

    const ProcessDriver & pd();

    const std::string& storage_name(size_t process_id) const;

    size_t process_id(const std::string& name) const;

    inline const std::vector<size_t>& batch_fillers() const
    { return _batch_filler_id_v; }

    inline const std::vector<larcv3::BatchDataType_t>& batch_types() const
    { return _batch_data_type_v; }

    // inline size_t num_batch_storage() const
    // { return _num_batch_storage; }

    void wait(std::chrono::microseconds duration);

  private:

    // enum TPRandomAccessMode_t { kTPRandomNo, kTPRandomEntry, kTPRandomBatch, kTPRandomUnknown};

    // int random_number(int range_min, int range_max);

    // void terminate_threads();

    bool set_batch_storage();
    bool begin_batch();
    bool end_batch();

    void manager_batch_process();

    bool _batch_process_(std::vector< size_t>  start_entry, size_t local_batch_entry);

    bool _processing;
    bool _configured;
    std::vector<size_t> _next_index_v;

    std::vector<std::string> _input_fname_v;
    size_t _batch_global_counter;

    // This controls the different pieces of a batch (data, image, label, pid, etc)
    std::vector<size_t> _batch_filler_id_v;
    std::vector<larcv3::BatchDataType_t> _batch_data_type_v;

    // Each QueueProcessor gets one process driver object.
    // We assume that the batch_process call can be parallelized with OpenMP
    larcv3::ProcessDriver _driver;
    
    // main thread
    // thread-wise variables
    // std::vector<size_t> _current_storage_id;
    // std::vector<size_t> _thread_exec_ctr_v;
    // std::vector<size_t> _batch_size_v;
    // std::vector<size_t> _valid_counter_v;
    // std::vector<size_t> _lifetime_valid_counter_v;
    // process-wise variables
    std::vector<std::string> _process_name_v;

    // storage-wise variables
    // std::vector<larcv3::BatchDataState_t> _batch_state_v;
    std::vector<size_t> _current_batch_entries_v;
    std::vector<larcv3::EventID> _current_batch_events_v;
    std::vector<size_t> _next_batch_entries_v;
    std::vector<larcv3::EventID> _next_batch_events_v;

    // std::vector<size_t> _batch_global_id;
  };

}

#endif
/** @} */ // end of doxygen group

