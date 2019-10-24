/**
 * \file IOManager.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv3::IOManager
 *
 * @author drinkingkazu
 * @author coreyjadams
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_IOMANAGER_H
#define __LARCV3DATAFORMAT_IOMANAGER_H

#include <iostream>
#include <map>
#include <set>

#include "hdf5.h"

#ifdef LARCV_MPI
#include <mpi.h>
#endif

#include "larcv3/core/base/larcv_base.h"
#include "larcv3/core/base/larbys.h"
#include "larcv3/core/base/PSet.h"
#include "larcv3/core/dataformat/EventBase.h"
#include "larcv3/core/dataformat/EventID.h"

//#include "ProductMap.h"
namespace larcv3 {


  /**
    \class IOManager
    \brief LArCV3 file IO hanlder class: it can read/write LArCV3 file.
  */
  class IOManager : public larcv3::larcv_base {

  public:
    /// Three IO modes: read, write, or read-and-write
    enum IOMode_t { kREAD, kWRITE, kBOTH };
    enum ProductStatus_t {kUnknown, kInputFileUnread, kInputFileRead, kOutputOnly, kVirtual};

    /// Default constructor
    IOManager(IOMode_t mode = kREAD, std::string name = "IOManager");

    /// Configuration PSet construction so you don't have to call setter functions
    IOManager(const PSet& cfg);

    /// Configuration PSet file construction so you don't have to call setter functions
    IOManager(std::string config_file, std::string name = "IOManager");

    /// Destructor
    ~IOManager();
    /// IO mode accessor
    IOMode_t io_mode() const { return _io_mode;}
    void reset();
    void add_in_file(const std::string filename, const std::string dirname = "");
    void clear_in_file();
    void set_core_driver(const bool opt = true);
    void set_out_file(const std::string name);
    ProducerID_t producer_id(const ProducerName_t& name) const;
    std::string product_type(const size_t id) const;
    void configure(const PSet& cfg);
    bool initialize(int color=0);
    bool read_entry(const size_t index, bool force_reload = false);
    bool save_entry();
    void finalize();
    void clear_entry();
    void set_id(const long run, const long subrun, const long event);
    size_t current_entry() const { return _in_index; }

    size_t get_n_entries_out() const
    { return _out_entries;}

    std::string get_file_out_name() const
    { return _out_file_name;}

    size_t get_n_entries() const
    { return (_in_entries_total ? _in_entries_total : _out_entries); }

    EventBase* get_data(const std::string& type, const std::string& producer);
    EventBase* get_data(const ProducerID_t id);
    //
    // Some template class getter for auto-cast
    //

    template <class T>
    inline T& get_data(const std::string& producer)
    { return *((T*)(this->get_data(product_unique_name<T>(), producer))); }

    template <class T>
    inline T& get_data(const ProducerID_t id)
    {
      if (id >= _product_type_v.size()) {
        LARCV_CRITICAL() << "Invalid producer id: " << id << " requested " << std::endl;
        throw larbys();
      }
      auto ptr = this->get_data(id);
      if (product_unique_name<T>() != _product_type_v[id]) {
        LARCV_CRITICAL() << "Unmatched type (in memory type = " << _product_type_v[id]
                         << " while specialization type = " << product_unique_name<T>()
                         << std::endl;
        throw larbys();
      }
      return *((T*)(ptr));
    }

    const EventID& event_id() const { return ( _set_event_id.valid() ? _set_event_id : _event_id ); }

    const EventID& last_event_id() const { return _last_event_id; }

    const std::vector<std::string> producer_list(const std::string type) const
    {
      std::vector<std::string> res;
      for (auto const& key_value : _key_list) {
        if (key_value.first.first != type) continue;
        res.push_back(key_value.first.second);
      }
      return res;
    }

    const std::vector<std::string> product_list() const
    {
      std::vector<std::string> res;
      for (auto const& key_value : _key_list) {
        res.push_back(key_value.first.first);
      }
      return res;
    }

    const std::vector<std::string>& file_list() const
    { return _in_file_v; }



  protected:
    void   set_id();
    void   prepare_input();
    size_t register_producer(const ProducerName_t& name);

    void open_new_input_file(std::string filename);
    void read_current_event_id();

    void append_event_id();

    // Closes the objects currently open in file with id fid
    int close_all_objects(hid_t fid);

    // The hdf5 model enforces the same number of entries per group,
    // since the entry list is defined per file.

    // IOManager supports only one output file, but multiple input files.
    // Files are checked for consistency in which groups are present.

    // General Parameters
    IOMode_t    _io_mode;
    bool        _prepared;
    // This can override the compression level across the entire file:
    uint _compression_override;


    // Parameters controlling output file large scale tracking:
    // ID of the output file:
    hid_t  _out_file;
    // Current output index
    size_t      _out_index;
    // Total output entries
    size_t      _out_entries;
    // Name of the output file
    std::string _out_file_name;

    // Parameters controllign input file large scale tracking:
    // Current index in the input files
    size_t      _in_index;
    size_t      _current_offset;
    // Total number of input entries
    size_t      _in_entries_total;
    // Index of currently active file
    size_t      _in_active_file_index;
    // List of input file names:
    std::vector<std::string>        _in_file_v;
    // List of input directory names:
    std::vector<std::string>        _in_dir_v;
    // Currently open file:
    hid_t       _in_open_file;
    // List of total entries in input files
    std::vector<size_t>             _in_entries_v;

    // Parameters for the event ID management:
    EventID   _event_id;
    EventID   _set_event_id;
    EventID   _last_event_id;

    hid_t     _active_in_event_id_dataset;
    hid_t     _active_in_event_id_dataspace;

    hid_t data_group;
    hid_t events_group;


    // Parameters controlling the internals of an event.
    // Dealing with input groups:


    // Output groups.  Since these are stored and need to be initialized, this is a member:
    std::vector<hid_t> _out_group_v;


    // Key list keeps track of the mapping from producer/product to an id:
    std::map<larcv3::ProducerName_t, larcv3::ProducerID_t> _key_list;
    // This is the key list for all producers in the input file only:
    std::map<larcv3::ProducerName_t, larcv3::ProducerID_t> _in_key_list;


    // Hold a copy of the file access property list:
    hid_t  _fapl; //FileAccPropList 

    std::map<std::string, hid_t> _groups;


    // Keeping track of products and producers:
    size_t                          _product_ctr;
    std::vector<larcv3::EventBase*> _product_ptr_v;
    std::vector<std::string>        _product_type_v;
    std::vector<std::string>        _producer_name_v;
    std::vector<ProductStatus_t>    _product_status_v;

    // General IO:
    std::map<std::string,std::set<std::string> > _store_only;
    std::map<std::string,std::set<std::string> > _read_only;
    std::vector<bool> _store_id_bool;
    std::vector<bool> _read_id_bool;
    bool _h5_core_driver;


    // IOManager has to control the EventID dataset it's self for the output file.
    hid_t  _out_event_id_ds;  // dataset
    hid_t _event_id_datatype; // datatype
    hid_t xfer_plist_id;

    // Internal bookkeeping for when the input file switches:
    bool _force_reopen_groups;

    // MPI Variables:
#ifdef LARCV_MPI

    MPI_Comm _private_comm;
    int _private_rank;
    int _private_size;

#endif

  };

}

#endif
/** @} */ // end of doxygen group
    