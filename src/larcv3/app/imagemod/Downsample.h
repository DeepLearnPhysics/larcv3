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

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet&);

    // List of input producers:
    std::vector<std::string> _input_producer_v;
    // List of input datatypes:
    std::vector<std::string> _input_product_v;
    // List of output producers:
    std::vector<std::string> _output_producer_v;
    // List of downsampling amount per projection ID (or one global Downsample for all)
    std::vector<size_t>       _downsamples_v;
    // Pooling Types
    std::vector<int> _pool_types_v;

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

}

#endif
/** @} */ // end of doxygen group

