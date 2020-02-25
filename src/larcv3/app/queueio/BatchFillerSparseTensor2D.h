/**
 * \file BatchFillerSparseTensor2D.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerSparseTensor2D
 *
 * @author cadams
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERSPARSETENSOR2D_H__
#define __LARCV3THREADIO_BATCHFILLERSPARSETENSOR2D_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

#include "larcv3/core/dataformat/EventSparseTensor.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor2D ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerSparseTensor2D : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerSparseTensor2D(const std::string name="BatchFillerSparseTensor2D");

    /// Default destructor
    ~BatchFillerSparseTensor2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    size_t set_data_size(const EventSparseTensor2D& image_data);

    std::string _tensor2d_producer;
    size_t _max_voxels;
    float _unfilled_voxel_value;
    std::vector<size_t> _slice_v;



    std::vector<float>  _entry_data;
    size_t _num_channels;
    bool _allow_empty;
    bool _include_values;
    bool _augment;
  };

  /**
     \class larcv3::BatchFillerSparseTensor2DFactory
     \brief A concrete factory class for larcv3::BatchFillerSparseTensor2D
  */
  class BatchFillerSparseTensor2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerSparseTensor2DProcessFactory() { ProcessFactory::get().add_factory("BatchFillerSparseTensor2D",this); }
    /// dtor
    ~BatchFillerSparseTensor2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerSparseTensor2D(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group

