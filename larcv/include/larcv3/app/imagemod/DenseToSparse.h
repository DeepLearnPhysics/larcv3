/**
 * \file DenseToSparse.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class DenseToSparse
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_DENSETOSPARSE_H__
#define __LARCV3_DENSETOSPARSE_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class DenseToSparse ... these comments are used to generate
     doxygen documentation!
  */
  class DenseToSparse : public ProcessBase {

  public:

    /// Default constructor
    DenseToSparse(const std::string name="DenseToSparse");

    /// Default destructor
    ~DenseToSparse(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"ReferenceProducer", std::string()}
        };
        return c;
    }

  private:

    json config;

    template <class dataproduct_in, class dataproduct_out>
    bool process_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        std::string ref_producer);

  };

  /**
     \class larcv3::DenseToSparseFactory
     \brief A concrete factory class for larcv3::DenseToSparse
  */
  class DenseToSparseProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DenseToSparseProcessFactory() { ProcessFactory::get().add_factory("DenseToSparse",this); }
    /// dtor
    ~DenseToSparseProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new DenseToSparse(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_dense_to_sparse(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

