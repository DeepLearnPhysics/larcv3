/**
 * \file ProcessDriver.h
 *
 * \ingroup core_Processor
 *
 * \brief Class def header for a class larcv3::ProcessDriver
 *
 * @author drinkingkazu, cadams
 */

/** \addtogroup core_Processor

    @{*/
#ifndef __LARCV3PROCESSOR_PROCESSDRIVER_H
#define __LARCV3PROCESSOR_PROCESSDRIVER_H

#include <vector>
#include "larcv3/core/dataformat/IOManager.h"
#include "larcv3/core/processor/ProcessBase.h"

namespace larcv3 {
  /**
     \class ProcessDriver
     @brief Analysis event-loop driver that executes larcv3::ProcessBase inherit "process modules".
     It uses larcv3::IOManager behind the scene to run the event loop.\n
     Responsible for propagating a structured configuration parameter sets to configure each process module.\n
     The configuration of larcv3::ProcessDriver itself can contain larcv3::IOManager configuration.
  */
  class ProcessDriver : public larcv_base {

  public:

    /// Default constructor
    ProcessDriver(std::string name="ProcessDriver");
    /// Default destructor
    ~ProcessDriver(){}

    //
    // Configuration setter/modifiers
    //
    /// Configure itself (mainly larcv3::IOManager) and also attached process modules.
    void configure(const json& cfg);

    /// Get the current configuration
    const json & get_config() const {return config;}

    /// When needs to override the list of input files from what's specified in the configuration
    void override_input_file(const std::vector<std::string>& flist);
    /// When needs to override the name of output data file from what's specified in the configuration
    void override_output_file(const std::string fname);
    /// When needs to override the randomized event access in IO from what's specified in the configuration
    void random_access(int flag) { config["RandomAccess"] = flag; }

    //
    // Process flow execution methods
    //
    /// Resets the states & all variables. Attached process modules would be removed.
    void reset();
    /// Must be called after configure. This initializes larcv3::IOManager (open files) and initialize attached process modules.
    void initialize(int color=0);
    /// Processes the "next entry" automatically (you don't have to control the counter to specify which entry)
    bool process_entry();
    /// Processes multiple entries (specified in the 2nd argument) from a specified entry (1st argument)
    void batch_process(size_t start_entry=0, size_t num_entries=0);
    /**
       @brief Processes a specific entry specified in the argument.
       If calling this function with the same entry twice, the 2nd bloolean can be used to force re-loading data from disk.
    */
    bool process_entry(size_t entry,bool force_reload=false);
    /// Must be called after initialize() or process_entry/batch_process. Closes IO and calls finalize method of process modules.
    void finalize();

    inline void clear_entry(){_io.clear_entry();}

    //
    // Information setter method
    //
    /// A method to force re-set the run/subrun/event ID of a currently processed event, useful when "creating an event".
    void set_id(size_t run, size_t subrun, size_t event)
    { _io.set_id(run,subrun,event); }
    //
    // Information access methods
    //
    /// Returns larcv3::EventBase object that contains an "ID" (run/event integers)
    inline const EventID& event_id() const { return _io.event_id(); }

    /// Returns a unique ID (integer) assigned for a process module (provide the module's name in argument)
    ProcessID_t process_id(std::string name) const;
    /// Returns the set of process modules' name
    std::vector<std::string> process_names() const;
    /// Returns a mapping between process modules' name and their unique process ID number
    const std::map<std::string,size_t>& process_map() const;
    /// Returns an attached process module's pointer given a unique ID in the argument
    const ProcessBase* process_ptr(ProcessID_t id) const;
    /// Returns read-only larcv3::IOManager instance
    const IOManager& io() const { return _io; }
    /// When run in random-access IO mode, returns original event entry number for a randomized index number
    size_t get_tree_index( size_t entry ) const;
    /// Returns true if after any entry is processed (process_entry/batch_process) but not yet finalized
    inline bool processing() const { return _processing; }

    /// Returns the default configuration for Process Driver
    static json default_config(){
        json c = {
            {"Verbosity", logger::default_level()},
            {"IOManager", IOManager::default_config()},
            {"ProcessType", std::vector<std::string>()},
            {"ProcessName", std::vector<std::string>()},
            {"ProcessList", json()},
            {"EnableFilter", false},
            {"RandomAccess", false},
            {"RandomSeed",   NULL},
            {"StartEntry", 0},
            {"NumEntries", 0},
        };
        return c;
    }

  protected:

    json config;

    bool _process_entry_();
    size_t _batch_start_entry;
    size_t _batch_num_entry;
    size_t _current_entry;
    std::vector<size_t> _access_entry_v;
    IOManager _io;
    std::map<std::string,larcv3::ProcessID_t> _proc_m;
    std::vector<larcv3::ProcessBase*> _proc_v;
    bool _processing;
  };
}
#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_processdriver(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group
