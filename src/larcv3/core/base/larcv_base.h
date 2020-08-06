/**
 * \file larcv_base.h
 *
 * \ingroup core_Base
 *
 * \brief Class definition file of larcv3::larcv_base
 *
 * @author Kazu - Nevis 2015
 */

/** \addtogroup core_Base

    @{*/

#ifndef __LARCV3BASE_LARCV_BASE_H__
#define __LARCV3BASE_LARCV_BASE_H__

#include <vector>
#include "larcv3/core/base/larcv_logger.h"
#include "pybind11_json/pybind11_json.hpp"
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;



namespace larcv3 {

  /**
    \class larcv_base
    Framework base class equipped with a logger class
  */
  class larcv_base {

  public:

    /// Default constructor
    larcv_base(const std::string logger_name="larcv_base")
      : _logger(nullptr)
    { _logger = &(::larcv3::logger::get(logger_name)); }

    /// Default copy constructor
    larcv_base(const larcv_base &original) : _logger(original._logger) {}

    /// Default destructor
    virtual ~larcv_base(){};

    /// Logger getter
    inline const larcv3::logger& logger() const
    { return *_logger; }

    /// Verbosity level
    void set_verbosity(::larcv3::msg::Level_t level)
    { _logger->set(level); }

    /// Name getter, defined in a logger instance attribute
    const std::string& name() const
    { return logger().name(); }

    /// Base config getter
    static json default_config(){
      json j = {{ "larcv_base" ,
        {"", ""}
      }};
      return j;
    };
    


  private:

    larcv3::logger *_logger;   ///< logger

  };
}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_larcv_base(pybind11::module m);
#endif

#endif

/** @} */ // end of doxygen group
