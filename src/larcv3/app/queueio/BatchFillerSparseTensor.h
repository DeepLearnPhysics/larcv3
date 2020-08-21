/**
 * \file BatchFillerSparseTensor.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerSparseTensor2D
 *
 * @author cadams
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERSPARSETENSOR_H__
#define __LARCV3THREADIO_BATCHFILLERSPARSETENSOR_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

#include "larcv3/core/dataformat/EventSparseTensor.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor ... these comments are used to generate
     doxygen documentation!
  */
  template<size_t dimension>
  class BatchFillerSparseTensor : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerSparseTensor(const std::string name="BatchFillerSparseTensor");

    /// Default destructor
    ~BatchFillerSparseTensor(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
      json c = {
        {"TensorProducer", std::string()},
        {"Augment", true},
        {"MaxVoxels", 0},
        {"UnfilledVoxelValue", -999.},
        {"Channels", std::vector<int>()},
        {"IncludeValues", true},
      };
      return c;
    }

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    json config;

    size_t set_data_size(const EventSparseTensor<dimension>& image_data);
    int _check_projection(const int & projection_id, const std::vector<int> & _slice_v);

    // std::string _tensor_producer;
    // size_t _max_voxels;
    // float _unfilled_voxel_value;
    // std::vector<size_t> _slice_v;



    std::vector<float>  _entry_data;
    size_t _num_channels;
    bool _allow_empty;
    // bool _include_values;
    // bool _augment;
  };

  typedef BatchFillerSparseTensor<2>  BatchFillerSparseTensor2D;
  typedef BatchFillerSparseTensor<3>  BatchFillerSparseTensor3D;

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::BatchFillerSparseTensor2D>() {
    return "BatchFillerSparseTensor2D";
  }
  template<> inline std::string product_unique_name<larcv3::BatchFillerSparseTensor3D>() {
    return "BatchFillerSparseTensor3D";
  }

  /**
     \class larcv3::BatchFillerSparseTensorFactory
     \brief A concrete factory class for larcv3::BatchFillerSparseTensor
  */
  template<size_t dimension>
  class BatchFillerSparseTensorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerSparseTensorProcessFactory() {
      ProcessFactory::get().add_factory(product_unique_name<larcv3::BatchFillerSparseTensor<dimension>>(),
                                        this);
    }
    /// dtor
    ~BatchFillerSparseTensorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerSparseTensor<dimension>(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group

