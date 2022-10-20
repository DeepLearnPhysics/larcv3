/**
 * \file larcv_base.h
 *
 * \ingroup core_Base
 *
 * \brief Class definition file of larcv3::larcv_base
 *
 * @author Kazu - Nevis 2015
 * @author Corey - ANL 2019
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

    /**
     * @brief      Constructs a new instance.
     *
     * @param[in]  logger_name  The logger name
     */
    larcv_base(const std::string logger_name="larcv_base")
      : _logger(nullptr)
    { _logger = &(::larcv3::logger::get(logger_name)); }

    /**
     * @brief      Constructs a new instance.
     *
     * @param[in]  original  The original
     */
    larcv_base(const larcv_base &original) : _logger(original._logger) {}

    /**
     * @brief      Destroys the object.
     */
    virtual ~larcv_base(){};

    /**
     * @brief      Get const reference to logger
     *
     * @return     { description_of_the_return_value }
     */
    inline const larcv3::logger& logger() const
    { return *_logger; }

    /**
     * @brief      Sets the verbosity.
     *
     * @param[in]  level  The level
     */
    void set_verbosity(::larcv3::msg::Level_t level)
    { _logger->set(level); }

    /**
     * @brief       Name getter, defined in a logger instance attribute
     *
     * @return     name of the logger
     */
    const std::string& name() const
    { return logger().name(); }

    /**
     * @brief      Base config getter
     *
     * @return     json config
     */
    static json default_config(){
      json j = {{ "larcv_base" ,
        {"", ""}
      }};
      return j;
    };
    
  protected:
    /**
     * @brief      Augments the default configuration.
     *
     * @param[in]  default_config  The default configuration
     * @param[in]  user_config     The user configuration
     *
     * @return     Recursively updates the default config (first argument)
     *  with a user config (second argument).  ANy items in the user_config are either
     *  added to the default config or replace the existing keys.
     */
    json augment_default_config(const json& default_config, const json& user_config);

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
