/**
 * \file BBoxFromCluster.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class BBoxFromCluster
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV_BBOXFROMCLUSTER_H__
#define __LARCV_BBOXFROMCLUSTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BBoxFromCluster ... these comments are used to generate
     doxygen documentation!
  */
  class BBoxFromCluster : public ProcessBase {

  public:

    /// Default constructor
    BBoxFromCluster(const std::string name="BBoxFromCluster");

    /// Default destructor
    ~BBoxFromCluster(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"Threshold", 0.0},
          {"MinVoxels", 1},
        };
        return c;
    }

  private:

    json config;

    template<size_t dimension>
    bool bbox_from_cluster(IOManager& mgr, std::string producer, std::string output_producer);

  };

  /**
     \class larcv3::BBoxFromClusterProcessFactory
     \brief A concrete factory class for larcv3::BBoxFromCluster
  */


  class BBoxFromClusterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BBoxFromClusterProcessFactory() { ProcessFactory::get().add_factory("BBoxFromCluster",this); }
    /// dtor
    ~BBoxFromClusterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BBoxFromCluster(instance_name); }
  };


}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_bbox_from_cluster(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

