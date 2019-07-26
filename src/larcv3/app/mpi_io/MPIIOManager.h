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
#ifndef __LARCV3DATAFORMAT_MPIIOMANAGER_H
#define __LARCV3DATAFORMAT_MPIIOMANAGER_H

#include <iostream>
#include <map>
#include <set>
#include <mpi.h>


#include "larcv3/core/dataformat/IOManager.h"



//#include "ProductMap.h"
namespace larcv3 {


  /**
    \class IOManager
    \brief LArCV3 file IO hanlder class: it can read/write LArCV3 file.
  */
  class MPIIOManager : public IOManager {

  public:

    /// Default constructor
    MPIIOManager(IOMode_t mode = kREAD, std::string name = "MPIIOManager");

    /// Configuration PSet construction so you don't have to call setter functions
    MPIIOManager(const PSet& cfg);

    /// Configuration PSet file construction so you don't have to call setter functions
    MPIIOManager(std::string config_file, std::string name = "IOManager");

    /// Destructor
    ~MPIIOManager();

    bool initialize();

  private:

    void open_new_input_file(std::string filename);
   
  };

}

#endif
/** @} */ // end of doxygen group
