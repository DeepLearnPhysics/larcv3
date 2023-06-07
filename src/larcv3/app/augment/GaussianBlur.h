/**
 * \file GaussianBlur.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class GaussianBlur
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_GAUSSIANBLUR_H__
#define __LARCV3_GAUSSIANBLUR_H__

#include <random>

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class GaussianBlur ... these comments are used to generate
     doxygen documentation!
  */
  class GaussianBlur : public ProcessBase {

  public:

    /// Default constructor
    GaussianBlur(const std::string name="GaussianBlur");

    /// Default destructor
    ~GaussianBlur(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"Sigma", float(0.0)},
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
        std::normal_distribution<float>,
        std::default_random_engine generator);

    template <class dataproduct>
    bool process_sparse_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        std::normal_distribution<float>,
        std::default_random_engine generator);

  };

  /**
     \class larcv3::GaussianBlurFactory
     \brief A concrete factory class for larcv3::GaussianBlur
  */
  class GaussianBlurProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    GaussianBlurProcessFactory() { ProcessFactory::get().add_factory("GaussianBlur",this); }
    /// dtor
    ~GaussianBlurProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new GaussianBlur(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_gaussian_blur(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

