/**
 * \file SparseToDense.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class SparseToDense
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_SPARSETODENSE_H__
#define __LARCV3_SPARSETODENSE_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class SparseToDense ... these comments are used to generate
     doxygen documentation!
  */
  class SparseToDense : public ProcessBase {

  public:

    /// Default constructor
    SparseToDense(const std::string name="SparseToDense");

    /// Default destructor
    ~SparseToDense(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
        };
        return c;
    }

  private:

    json config;

    template <class dataproduct_in, class dataproduct_out>
    bool process_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer);

  };

  /**
     \class larcv3::SparseToDenseFactory
     \brief A concrete factory class for larcv3::SparseToDense
  */
  class SparseToDenseProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SparseToDenseProcessFactory() { ProcessFactory::get().add_factory("SparseToDense",this); }
    /// dtor
    ~SparseToDenseProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SparseToDense(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_sparse_to_dense(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

