/**
 * \file BatchFillerTensor3D.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERTENSOR3D_H__
#define __LARCV3THREADIO_BATCHFILLERTENSOR3D_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"
#include "RandomCropper.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerTensor3D : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerTensor3D(const std::string name="BatchFillerTensor3D");
    
    /// Default destructor
    ~BatchFillerTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    // size_t set_data_size(const SparseTensor3D& image_data);
    void assert_dimension(const SparseTensor3D& image_data) const;

    std::string _tensor3d_producer;
    size_t _nx;
    size_t _ny;
    size_t _nz;
    std::vector<float>  _entry_data;
    size_t _num_channel;
    float _voxel_base_value;
    bool _allow_empty;
  };

  /**
     \class larcv3::BatchFillerTensor3DFactory
     \brief A concrete factory class for larcv3::BatchFillerTensor3D
  */
  class BatchFillerTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerTensor3DProcessFactory() { ProcessFactory::get().add_factory("BatchFillerTensor3D",this); }
    /// dtor
    ~BatchFillerTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerTensor3D(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group 

