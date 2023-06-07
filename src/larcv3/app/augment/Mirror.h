/**
 * \file Mirror.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Mirror
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_MIRROR_H__
#define __LARCV3_MIRROR_H__

#include <random>

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class Mirror ... these comments are used to generate
     doxygen documentation!
  */
  class Mirror : public ProcessBase {

  public:

    /// Default constructor
    Mirror(const std::string name="Mirror");

    /// Default destructor
    ~Mirror(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"Axes", std::vector<size_t>()},
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
        std::vector<uint>);

    template <class dataproduct>
    bool process_sparse_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        std::vector<uint>);

  };

  /**
     \class larcv3::MirrorFactory
     \brief A concrete factory class for larcv3::Mirror
  */
  class MirrorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MirrorProcessFactory() { ProcessFactory::get().add_factory("Mirror",this); }
    /// dtor
    ~MirrorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Mirror(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_mirror(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

