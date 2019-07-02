/**
 * \file Threshold.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Threshold
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_THRESHOLD_H__
#define __LARCV3_THRESHOLD_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class Threshold ... these comments are used to generate
     doxygen documentation!
  */
  class Threshold : public ProcessBase {

  public:

    /// Default constructor
    Threshold(const std::string name="Threshold");

    /// Default destructor
    ~Threshold(){}

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
    // List of thresholds per projection ID (or one global threshold for all)
    std::vector<float>       _thresholds_v;
  };

  /**
     \class larcv3::ThresholdFactory
     \brief A concrete factory class for larcv3::Threshold
  */
  class ThresholdProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ThresholdProcessFactory() { ProcessFactory::get().add_factory("Threshold",this); }
    /// dtor
    ~ThresholdProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Threshold(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

