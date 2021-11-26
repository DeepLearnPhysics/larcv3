/**
 * \file EmptyTensorFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EmptyTensorFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV3FILTER_EMPTYTENSORFILTER_H__
#define __LARCV3FILTER_EMPTYTENSORFILTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class EmptyTensorFilter ... these comments are used to generate
     doxygen documentation!
  */
  class EmptyTensorFilter : public larcv3::ProcessBase {

  public:
    
    /// Default constructor
    EmptyTensorFilter(const std::string name="EmptyTensorFilter");
    
    /// Default destructor
    ~EmptyTensorFilter(){}

    void configure(const larcv3::PSet&);

    void initialize();

    bool process(larcv3::IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<std::string> _tensor3d_producer_v, _tensor2d_producer_v;
    std::vector<size_t>      _min_voxel2d_count_v;
    std::vector<float>       _min_voxel2d_value_v;
    std::vector<size_t>      _min_voxel3d_count_v;
    std::vector<float>       _min_voxel3d_value_v;
  };

  /**
     \class larcv3::EmptyTensorFilterFactory
     \brief A concrete factory class for larcv3::EmptyTensorFilter
  */
  class EmptyTensorFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EmptyTensorFilterProcessFactory() { ProcessFactory::get().add_factory("EmptyTensorFilter",this); }
    /// dtor
    ~EmptyTensorFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EmptyTensorFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

