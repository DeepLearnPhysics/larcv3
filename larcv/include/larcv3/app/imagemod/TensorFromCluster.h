/**
 * \file TensorFromCluster.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class TensorFromCluster
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV_TENSORFROMCLUSTER_H__
#define __LARCV_TENSORFROMCLUSTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class TensorFromCluster ... these comments are used to generate
     doxygen documentation!
  */
  class TensorFromCluster : public ProcessBase {

  public:

    /// Default constructor
    TensorFromCluster(const std::string name="TensorFromCluster");

    /// Default destructor
    ~TensorFromCluster(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product",  std::string()},
          {"OutputProducer", std::string()},
        };
        return c;
    }

  private:

    json config;


    template<class dataproduct_in, class dataproduct_out>
    bool merge_clusters(IOManager& mgr, std::string producer, std::string output_producer);

  };

  /**
     \class larcv3::TensorFromClusterFactory
     \brief A concrete factory class for larcv3::TensorFromCluster
  */


  class TensorFromClusterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    TensorFromClusterProcessFactory() { ProcessFactory::get().add_factory("TensorFromCluster",this); }
    /// dtor
    ~TensorFromClusterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new TensorFromCluster(instance_name); }
  };


}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_tensor_from_cluster(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

