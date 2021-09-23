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

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    static json default_config(){
      json c = {
        {"Producer", std::string()},
        {"MaxBoxes", 25},
        {"UnfilledBoxValue", 0.0},
        {"Channels", std::vector<int>()},
      };
      return c;
    }
  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    size_t set_data_size(const EventBBox<dimension>& image_data);
    int _check_projection(const int & projection_id);

    // std::string _bbox_producer;
    // size_t _max_boxes;
    // float _unfilled_box_value;
    // std::vector<size_t> _slice_v;

    json config;

    std::vector<float>  _entry_data;
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

#ifdef LARCV_INTERNAL
template <size_t dimension>
void init_bf_bbox_(pybind11::module m);

void init_bf_bbox(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group

