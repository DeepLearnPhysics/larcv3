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

    void configure(const json&);

    void initialize();

    bool process(larcv3::IOManager& mgr);

    void finalize();

    static json default_config() {
      json c = {
            {"TensorProducer", std::vector<std::string>()}, // Name of producer
            {"TensorType",     std::vector<std::string>()}, // Type of produce, eg, sparse2d
            {"MinVoxelCount",  std::vector<int>()}, // Min Number of voxels per projection
            {"MinVoxelValue",  std::vector<float>()}, // Min voxel value per projection
      };
      return c;
    }

  private:
    template<size_t dimension>
    bool process_tensor(IOManager & mgr, std::string producer, int min_voxel_count, float min_voxel_value);

    template<size_t dimension>
    bool process_sparse(IOManager & mgr, std::string producer, int min_voxel_count, float min_voxel_value);


    json config;

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

