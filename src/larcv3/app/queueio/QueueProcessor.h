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
    void configure(const std::string config_file, int color=0);

    // configure the processor from a PSet object    
    void configure(const PSet& cfg, int color=0);

    // Check if the processor is configured
    inline bool configured() const { return _configured;}

    // Go through all factories and pop the data
    void pop_current_data();

    // Set the next index to read/
    // This will just set the index_v object
    void set_next_index(size_t index);

    // Set the next set of indexes to read:
    void set_next_batch(const std::vector<size_t>& index_v);

    // Return true only if the fillers are preparing the next batch
    bool is_reading() const {return _processing;}

    // Get number of entries possible to read
    size_t get_n_entries() const;

    // Return the list of processed entries for current data
    const std::vector<size_t>& processed_entries() const;

    // Return the list of processed eventIDs for current data
    const std::vector<larcv3::EventID>& processed_events() const;

    // Read access to the process driver:
    const ProcessDriver & pd() const;

    const std::string& storage_name(size_t process_id) const;

    size_t process_id(const std::string& name) const;

    inline const std::vector<size_t>& batch_fillers() const
    { return _batch_filler_id_v; }

    inline const std::vector<larcv3::BatchDataType_t>& batch_types() const
    { return _batch_data_type_v; }

  private:

    bool set_batch_storage();
    bool begin_batch();
    bool end_batch();

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
    
    // List of processes for fillers:
    std::vector<std::string> _process_name_v;

    // Meta data about read entries variables
    std::vector<size_t> _current_batch_entries_v;
    std::vector<larcv3::EventID> _current_batch_events_v;
    std::vector<size_t> _next_batch_entries_v;
    std::vector<larcv3::EventID> _next_batch_events_v;

  };

}

#endif
/** @} */ // end of doxygen group

