/**
 * \file LArCVBaseUtilFunc.h
 *
 * \ingroup core_Base
 * 
 * \brief Utility functions in core_Base
 *
 * @author Kazu - Nevis 2015
 */

/** \addtogroup core_Base

    @{*/

#ifndef __LARCV3BASE_UTILFUNC_H__
#define __LARCV3BASE_UTILFUNC_H__

#include "larcv3/core/base/PSet.h"

namespace larcv3 {

  /// Given a configuration string, format to create larcv3::PSet
  //std::string FormatPSetString(std::string fname);
  /// Given a configuration file (full path), read & parse contents to create larcv3::PSet
  std::string ConfigFile2String(std::string fname);
  /// Given a configuration file (full path), create and return larcv3::PSet
  PSet CreatePSetFromFile(std::string fname,std::string cfg_name="cfg");

  bool mpi_enabled();

}

#endif
/** @} */ // end of doxygen group
