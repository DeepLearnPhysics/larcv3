#ifndef __LARCV3PROCESSOR_MPIPROCESSDRIVER_CXX
#define __LARCV3PROCESSOR_MPIPROCESSDRIVER_CXX

#include "larcv3/app/mpi_io/MPIProcessDriver.h"
#include <iomanip>
#include <random>
#include <sstream>
#include "larcv3/core/processor/ProcessFactory.h"
#include "larcv3/core/base/LArCVBaseUtilFunc.h"

namespace larcv3 {

  MPIProcessDriver::MPIProcessDriver(std::string name)
    : ProcessDriver(name)
      {
        if (_io) delete _io;
        _io = new MPIIOManager();
      }


  void MPIProcessDriver::configure(const PSet& cfg) {
    reset();

    ProcessDriver::configure(cfg);

    PSet io_config("Empty");
    if (cfg.contains_pset("IOManager"))
      io_config = cfg.get<larcv3::PSet>("IOManager");
    else if (cfg.contains_pset(std::string(name() + "IOManager")))
      io_config = cfg.get<larcv3::PSet>(name() + "IOManager");
    else {
      LARCV_CRITICAL() << "IOManager config not found!" << std::endl
                       << cfg.dump() << std::endl;
      throw larbys();
    }
    
    if (_io) delete _io;
    _io =  new MPIIOManager(io_config);


  }

void MPIProcessDriver::initialize(int color) {
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
  _io->initialize(color);

  // Handle invalid cases
  auto const nentries = _io->get_n_entries();
  auto const io_mode = _io->io_mode();

  // Block non readonly access in MPI mode
  // Not anticipating this as permanent, but:
  if (io_mode != IOManager::kREAD){
    LARCV_CRITICAL() << "Only read-only access is availble in MPI mode" << std::endl;
    throw larbys();
  }

  // Random access + write mode cannot be combined
  if (_random_access != 0 && io_mode == IOManager::kWRITE) {
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
    // if(_random_access)
    // std::random_shuffle(_access_entry_v.begin(),_access_entry_v.end());
    if (_random_access != 0) {
      unsigned int seed = 0;
      if (_random_access < 0)
        seed = std::chrono::system_clock::now().time_since_epoch().count();
      else
        seed = _random_access;
      std::shuffle(_access_entry_v.begin(), _access_entry_v.end(),
                   std::default_random_engine(seed));
    }
  }

  _current_entry = 0;
}


}  // namespace larcv3

#endif
