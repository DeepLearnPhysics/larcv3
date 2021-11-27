/**
 * @file LArCVBaseUtilFunc.h
 * @ingroup core_Base
 * @brief Utility functions in core_Base
 * @author Kazu - Nevis 2015
 */

#pragma once

#include "PSet.hh"

/** @addtogroup core_Base
 * @{
 */
namespace larcv3 
{
	/// Given a configuration string, format to create larcv3::PSet
	//std::string FormatPSetString(std::string fname);

	/// Given a configuration file (full path), read & parse contents to create larcv3::PSet
	std::string ConfigFile2String(std::string fname);

	/// Given a configuration file (full path), create and return larcv3::PSet
	PSet CreatePSetFromFile(std::string fname,std::string cfg_name="cfg");

	bool mpi_enabled();
}
/** @} */ // end of doxygen group