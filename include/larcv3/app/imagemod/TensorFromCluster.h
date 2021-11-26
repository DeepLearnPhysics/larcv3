/**
 * \file TensorFromCluster.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class TensorFromCluster
 *
 * @author kazuhiro, cadams
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
  template<size_t dimension> 
  class TensorFromCluster : public ProcessBase {

  public:

    /// Default constructor
    TensorFromCluster(const std::string name="TensorFromCluster");

    /// Default destructor
    ~TensorFromCluster(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);

    enum class PIType_t {
      kPITypeFixedPI,
      kPITypeInputVoxel,
      kPITypeClusterIndex,
      kPITypeUndefined
    };

    std::vector<std::string> _cluster_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<unsigned short> _pi_type_v;
    std::vector<float> _fixed_pi_v;
  };

  /**
     \class larcv3::TensorFromClusterFactory
     \brief A concrete factory class for larcv3::TensorFromCluster
  */


  class TensorFromCluster2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    TensorFromCluster2DProcessFactory() { ProcessFactory::get().add_factory("TensorFromCluster2D",this); }
    /// dtor
    ~TensorFromCluster2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new TensorFromCluster<2>(instance_name); }
  };

  class TensorFromCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    TensorFromCluster3DProcessFactory() { ProcessFactory::get().add_factory("TensorFromCluster3D",this); }
    /// dtor
    ~TensorFromCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new TensorFromCluster<3>(instance_name); }
  };


}

#endif
/** @} */ // end of doxygen group

