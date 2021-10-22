/**
 * \file BatchFillerTensor.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerTensor2D
 *
 * @author cadams
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERTENSOR_H__
#define __LARCV3THREADIO_BATCHFILLERTENSOR_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventTensor.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor ... these comments are used to generate
     doxygen documentation!
  */
  template<size_t dimension>
  class BatchFillerTensor : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerTensor(const std::string name="BatchFillerTensor");

    /// Default destructor
    ~BatchFillerTensor(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
      json c = {
        {"Producer", std::string()},
        {"TensorType", "sparse"},
        {"Augment", true},
        {"EmptyVoxelValue", 0.0},
        {"Channels", std::vector<int>()},
        {"AllowEmpty", true}
      };
      return c;
    }

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    json config;

    bool _process_sparse(IOManager& mgr);
    bool _process_dense(IOManager& mgr);
    int _check_projection(const size_t & projection_id);
    size_t _set_image_size(const EventTensor<dimension>& image_data);
    void _assert_dimension(const EventTensor<dimension>& image_data, const std::vector<size_t> &) const;

    // std::string _tensor_producer;
    // std::string _tensor_type;
    // size_t _rows;
    // size_t _cols;
    size_t _dims[dimension];  ///< Total number of voxels in each dimension
    // size_t _num_channels;
    // std::vector<size_t> _slice_v;
    size_t _max_ch;

    std::vector<float>  _entry_data;
  };

  typedef BatchFillerTensor<2>  BatchFillerTensor2D;
  typedef BatchFillerTensor<3>  BatchFillerTensor3D;

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::BatchFillerTensor2D>() {
    return "BatchFillerTensor2D";
  }
  template<> inline std::string product_unique_name<larcv3::BatchFillerTensor3D>() {
    return "BatchFillerTensor3D";
  }

  /**
     \class larcv3::BatchFillerTensorFactory
     \brief A concrete factory class for larcv3::BatchFillerTensor
  */
  template<size_t dimension>
  class BatchFillerTensorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerTensorProcessFactory() {
      ProcessFactory::get().add_factory(product_unique_name<larcv3::BatchFillerTensor<dimension>>(),
                                        this);
    }
    /// dtor
    ~BatchFillerTensorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerTensor<dimension>(instance_name);
    }
  };


}

#ifdef LARCV_INTERNAL
template <size_t dimension>
void init_bf_tensor_(pybind11::module m);

void init_bf_tensor(pybind11::module m);
#endif


#endif


/** @} */ // end of doxygen group
