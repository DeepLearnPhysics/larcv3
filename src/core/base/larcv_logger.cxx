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
      pybind11::class_<larcv3::logger>(m, "logger")
        .def(pybind11::init<const std::string &>(),pybind11::arg("name")="no_name")
             // pybind11::arg("name")="no_name")
        // .def("logger", &larcv3::logger::logger)
        // .def("set_verbosity", &larcv3::logger::set_verbosity)
        .def("name", &larcv3::logger::name)
        .def("set", &larcv3::logger::set)
        .def("level", &larcv3::logger::level)
        .def(pybind11::self < pybind11::self)
        .def("get", &larcv3::logger::get)
        .def("get_shared", &larcv3::logger::get_shared)
        // .def("default_level", (larcv3::msg::Level_t (larcv3::logger::*)()) &larcv3::logger::default_level)
        // .def("default_level", (void (larcv3::logger::*)(larcv3::msg::Level_t)) &larcv3::logger::default_level)
        ;

        ///TODO Finish the implementation of the functions here
}

/*

    


    /// Default logger level getter
    static msg::Level_t default_level() { return _level_default; }
    /// Default logger level setter (only affect future loggers)
    static void default_level(msg::Level_t l) { _level_default = l; }
    /// Force all loggers to change level
    static void force_level(msg::Level_t l)
    {
      default_level(l);
      for(auto& name_logger : *_logger_m) name_logger.second.set(l);
    }
  
    //
    // Verbosity level checker
    //
    inline bool debug   () const { return _level <= msg::kDEBUG;   }
    inline bool info    () const { return _level <= msg::kINFO;    }
    inline bool normal  () const { return _level <= msg::kNORMAL;  }
    inline bool warning () const { return _level <= msg::kWARNING; }
    inline bool error   () const { return _level <= msg::kERROR;   }
    /// Formatted message (simplest)
    std::ostream& send(const msg::Level_t) const;
    /// Formatted message (function name included)
    std::ostream& send(const msg::Level_t level,
           const std::string& function ) const;
    /// Formatted message (function name + line number)
    std::ostream& send(const msg::Level_t level,
           const std::string& function,
           const unsigned int line_num ) const;
    /// Formatted message (function name + line number + file name)
    std::ostream& send(const msg::Level_t level,
           const std::string& function,
           const unsigned int line_num,
           const std::string& file_name) const;
    
*/


#endif
