// /**
//  * \file Threshold.h
//  *
//  * \ingroup ImageMod
//  *
//  * \brief Class def header for a class Threshold
//  *
//  * @author cadams
//  */

// /** \addtogroup ImageMod

//     @{*/
// #ifndef __LARCV3_THRESHOLD_H__
// #define __LARCV3_THRESHOLD_H__

// #include "larcv3/core/processor/ProcessBase.h"
// #include "larcv3/core/processor/ProcessFactory.h"
// namespace larcv3 {

//   /**
//      \class ProcessBase
//      User defined class Threshold ... these comments are used to generate
//      doxygen documentation!
//   */
//   class Threshold : public ProcessBase {

//   public:

//     /// Default constructor
//     Threshold(const std::string name="Threshold");

//     /// Default destructor
//     ~Threshold(){}

//     void configure(const PSet&);

//     void initialize();

//     bool process(IOManager& mgr);

//     void finalize();

//   private:

//     void configure_labels(const PSet&);

//     std::vector<std::string> _input_producer_v;
//     std::vector<std::string> _output_producer_v;
//     std::vector<float>       _thresholds_v;
//   };

//   /**
//      \class larcv3::Cluster2DThresholdFactory
//      \brief A concrete factory class for larcv3::Threshold
//   */
//   class Cluster2DThresholdProcessFactory : public ProcessFactoryBase {
//   public:
//     /// ctor
//     Cluster2DThresholdProcessFactory() { ProcessFactory::get().add_factory("Threshold",this); }
//     /// dtor
//     ~Cluster2DThresholdProcessFactory() {}
//     /// creation method
//     ProcessBase* create(const std::string instance_name) { return new Threshold(instance_name); }
//   };

// }

// #endif
// /** @} */ // end of doxygen group

