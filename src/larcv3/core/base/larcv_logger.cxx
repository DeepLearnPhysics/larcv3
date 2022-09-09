#ifndef __LARCV3BASE_LOGGER_CXX__
#define __LARCV3BASE_LOGGER_CXX__

#include "larcv3/core/base/larcv_logger.h"
#include <pybind11/operators.h>


#include <mutex>
std::mutex __logger_mtx;
namespace larcv3 {

  logger *logger::_shared_logger = nullptr;
  
  std::map<std::string,logger> *logger::_logger_m = nullptr;

  msg::Level_t logger::_level_default = msg::kNORMAL;
  
  std::ostream& logger::send(const msg::Level_t level) const
  {
    __logger_mtx.lock();
    (*_ostrm)  << msg::kStringPrefix[level].c_str()
	       << "\033[0m ";
    __logger_mtx.unlock();
    return (*_ostrm);
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function ) const
  {
    auto& strm(send(level));
    strm << "\033[94m<" << _name << "::" << function.c_str() << ">\033[00m ";
    return strm;
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function,
			     const unsigned int line_num ) const
  {
    auto& strm(send(level));
    strm << "\033[94m<" << _name << "::" << function.c_str() << "::L" << line_num << ">\033[00m ";
    return strm;
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function,
			     const unsigned int line_num,
			     const std::string& file_name) const
  {
    auto& strm(send(level,function));
    // FIXME temporary operation to fetch file name from full path
    strm << file_name.substr(file_name.rfind("/")+1,file_name.size()).c_str() << "::L" << line_num << " ";
    return strm;
  }
  
  logger& logger::get_shared()
  {
    __logger_mtx.lock();
    if(!_shared_logger) _shared_logger = new logger("GLOBAL");
    __logger_mtx.unlock();
    return *_shared_logger;

  }
}


void init_logger(pybind11::module m){
      pybind11::class_<larcv3::logger> logger(m, "logger");

      logger.doc() = 
      R"pbdoc(
        logger
        ------

        C++ native logger utility, used to manage log messages from the C++ application code.

        For python logging, use the standard `logging` utilities.
      )pbdoc";

      logger.def(pybind11::init<const std::string &>(),pybind11::arg("name")="no_name",
      R"pbdoc(
        Construct a logger with a specified name.
      )pbdoc");
      logger.def("name", &larcv3::logger::name,
      R"pbdoc(
        Get the name associated with this logger.
      )pbdoc");
      logger.def("set", &larcv3::logger::set, pybind11::arg("level"),
      R"pbdoc(
          Set the level of this logger.
      )pbdoc");
      logger.def("level", &larcv3::logger::level,
      R"pbdoc(
          Get the level of this logger.
      )pbdoc");

}


#endif
