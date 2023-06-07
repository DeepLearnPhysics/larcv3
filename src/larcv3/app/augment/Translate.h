/**
 * \file Translate.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Translate
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_Translate_H__
#define __LARCV3_Translate_H__

#include <random>

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class Translate ... these comments are used to generate
     doxygen documentation!
  */
  class Translate : public ProcessBase {

  public:

    /// Default constructor
    Translate(const std::string name="Translate");

    /// Default destructor
    ~Translate(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"MaxShiftPerAxis", std::vector<int>()},
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
        std::vector<int>);

    template <class dataproduct>
    bool process_sparse_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        std::vector<int>);

  };

  /**
     \class larcv3::TranslateFactory
     \brief A concrete factory class for larcv3::Translate
  */
  class TranslateProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    TranslateProcessFactory() { ProcessFactory::get().add_factory("Translate",this); }
    /// dtor
    ~TranslateProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Translate(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_translate(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

