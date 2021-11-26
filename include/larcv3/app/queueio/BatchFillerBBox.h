/**
 * \file BatchFillerBBox.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerBBox2D
 *
 * @author cadams
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERBBOX_H__
#define __LARCV3THREADIO_BATCHFILLERBBOX_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

#include "larcv3/core/dataformat/EventBBox.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor ... these comments are used to generate
     doxygen documentation!
  */
  template<size_t dimension>
  class BatchFillerBBox : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerBBox(const std::string name="BatchFillerBBox");

    /// Default destructor
    ~BatchFillerBBox(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    size_t set_data_size(const EventBBox<dimension>& image_data);
    int _check_projection(const size_t & projection_id);

    std::string _bbox_producer;
    size_t _max_boxes;
    float _unfilled_box_value;
    std::vector<size_t> _slice_v;



    std::vector<float>  _entry_data;
    size_t _num_channels;
  };

  typedef BatchFillerBBox<2>  BatchFillerBBox2D;
  typedef BatchFillerBBox<3>  BatchFillerBBox3D;

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::BatchFillerBBox2D>() {
    return "BatchFillerBBox2D";
  }
  template<> inline std::string product_unique_name<larcv3::BatchFillerBBox3D>() {
    return "BatchFillerBBox3D";
  }

  /**
     \class larcv3::BatchFillerBBoxFactory
     \brief A concrete factory class for larcv3::BatchFillerBBox
  */
  template<size_t dimension>
  class BatchFillerBBoxProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerBBoxProcessFactory() {
      ProcessFactory::get().add_factory(product_unique_name<larcv3::BatchFillerBBox<dimension>>(),
                                        this);
    }
    /// dtor
    ~BatchFillerBBoxProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerBBox<dimension>(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group
