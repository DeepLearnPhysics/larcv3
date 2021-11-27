/**
 * @file larbys.hh
 * @ingroup core_Base
 * @brief Class def header for exception classes for larcv3 framework
 * @author kazuhiro tmw
 */

#pragma once

#ifdef LARCV_INTERNAL
  #include <pybind11/pybind11.h>
  void init_larbys(pybind11::module m);
#endif

#include <iostream>
#include <exception>

/** @addtogroup core_Base
 * @{
 */
namespace larcv3 
{
	/**
	 * @brief Throw insignificant larbys when you find nonesense
	 * 
	 */
	class larbys : public std::exception 
	{
	public:
		larbys(std::string msg="") 
		: std::exception()
		{
			_msg = "\033[93m";
			_msg += msg;
			_msg += "\033[00m";
		}
		virtual ~larbys() throw() 
		{}

		virtual const char* what() const throw()
		{ return _msg.c_str(); }

	private:
		std::string _msg;
	};
}
/** @} */ // end of doxygen group