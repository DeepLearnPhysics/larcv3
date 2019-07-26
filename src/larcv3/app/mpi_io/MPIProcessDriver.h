/**
 * \file MPIProcessDriver.h
 *
 * \ingroup core_Processor
 *
 * \brief Class def header for a class larcv3::MPIProcessDriver
 *
 * @author cadams
 */

/** \addtogroup core_Processor

    @{*/
#ifndef __LARCV3PROCESSOR_MPIPROCESSDRIVER_H
#define __LARCV3PROCESSOR_MPIPROCESSDRIVER_H

#include <vector>
#include "larcv3/app/mpi_io/MPIIOManager.h"
#include "larcv3/core/processor/ProcessDriver.h"

namespace larcv3 {
  /**
     \class ProcessDriver
     @brief Analysis event-loop driver that executes larcv3::ProcessBase inherit "process modules".
     It uses larcv3::IOManager behind the scene to run the event loop.\n
     Responsible for propagating a structured configuration parameter sets to configure each process module.\n
     The configuration of larcv3::ProcessDriver itself can contain larcv3::IOManager configuration.
  */
  class MPIProcessDriver : public ProcessDriver {

  public:

    /// Default constructor
    MPIProcessDriver(std::string name="MPIProcessDriver");
    /// Default destructor
    ~MPIProcessDriver(){}

    //
    // Process flow execution methods
    //
    /// Resets the states & all variables. Attached process modules would be removed.
    void reset();
    /// Must be called after configure. This initializes larcv3::IOManager (open files) and initialize attached process modules.
    void initialize(int color=0);
    void configure(const PSet& cfg);

    const IOManager& io() const { return  * _io; }

  protected:

  };


}

#endif
/** @} */ // end of doxygen group
