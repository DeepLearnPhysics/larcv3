/**
 * \file Downsample.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Downsample
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_DOWNSAMPLE_H__
#define __LARCV3_DOWNSAMPLE_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class Downsample ... these comments are used to generate
     doxygen documentation!
  */
  class Downsample : public ProcessBase {

  public:

    /// Default constructor
    Downsample(const std::string name="Downsample");

    /// Default destructor
    ~Downsample(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();


    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"Downsample", 0},
          {"PoolType", kPoolAverage},
        }
        return c;
    }


  private:

    json config;


  };

  /**
     \class larcv3::ThresholdFactory
     \brief A concrete factory class for larcv3::Downsample
  */
  class DownsampleProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DownsampleProcessFactory() { ProcessFactory::get().add_factory("Downsample",this); }
    /// dtor
    ~DownsampleProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Downsample(instance_name); }
  };

  #ifdef LARCV_INTERNAL
  #include <pybind11/pybind11.h>
  void init_downsample(pybind11::module m);
  #endif

}

#endif
/** @} */ // end of doxygen group

  