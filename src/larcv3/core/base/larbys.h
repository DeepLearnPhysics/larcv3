/**
 * \file larbys.h
 *
 * \ingroup core_Base
 * 
 * \brief Class def header for exception classes for larcv3 framework
 *
 * @author kazuhiro tmw
 */

/** \addtogroup core_Base

    @{*/
#ifndef __LARCV3BASE_LARBYS_H__
#define __LARCV3BASE_LARBYS_H__

#include <iostream>
#include <exception>

#include <pybind11/pybind11.h>



namespace larcv3 {

  /**
     \class larbys
     Throw insignificant larbys when you find nonesense 
  */
  class larbys : public std::exception {
    
  public:
    
    larbys(std::string msg="") : std::exception()
    {
      _msg = "\033[93m";
      _msg += msg;
      _msg += "\033[00m";
    }

    virtual ~larbys() throw() {}
    virtual const char* what() const throw()
    { return _msg.c_str(); }

  private:
    
    std::string _msg;
  };
}

void init_larbys(pybind11::module m);


// PYBIND11_MODULE(larcv, m) {
//   init_larbys(m);
// }

#endif
/** @} */ // end of doxygen group 

