/**
 * \file Normalize.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Normalize
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_NORMALIZE_H__
#define __LARCV3_NORMALIZE_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class Normalize ... these comments are used to generate
     doxygen documentation!
  */
  class Normalize : public ProcessBase {

  public:

    /// Default constructor
    Normalize(const std::string name="Normalize");

    /// Default destructor
    ~Normalize(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer",       std::string()},
          {"Product",        std::string()},
          {"OutputProducer", std::string()},
          {"Mean",           float(0.0)},
          {"Std",            float(1.0)},
        };
        return c;
    }

  private:

    json config;

    template <class dataproduct>
    bool process_dense_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        float target_mean,
        float target_std);

    template <class dataproduct>
    bool process_sparse_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        float target_mean,
        float target_std);

  };

  /**
     \class larcv3::ThresholdFactory
     \brief A concrete factory class for larcv3::Normalize
  */
  class NormalizeProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    NormalizeProcessFactory() { ProcessFactory::get().add_factory("Normalize",this); }
    /// dtor
    ~NormalizeProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Normalize(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_normalize(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

