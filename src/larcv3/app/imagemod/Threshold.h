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

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"Threshold", std::vector<float>() = {0.0}},
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
        const std::vector<float>& threshold);

    template <class dataproduct>
    bool process_sparse_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        const std::vector<float>& threshold);

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

