#ifndef __LARCV3THREADIO_BATCHFILLERBBOX_CXX__
#define __LARCV3THREADIO_BATCHFILLERBBOX_CXX__

#include "BatchFillerBBox.h"

#include <random>

namespace larcv3 {

  /// Global larcv3::BatchFillerBBoxProcessFactory to register BatchFillerBBox
  static BatchFillerBBoxProcessFactory<2>
      __global_BatchFillerBBox2DProcessFactory__;
  static BatchFillerBBoxProcessFactory<3>
      __global_BatchFillerBBox3DProcessFactory__;

  template<size_t dimension>
  BatchFillerBBox<dimension>::BatchFillerBBox(const std::string name)
      : BatchFillerTemplate<float>(name) {}

  template<size_t dimension>
  void BatchFillerBBox<dimension>::configure(const PSet& cfg) {


    LARCV_DEBUG() << "start" << std::endl;
    _bbox_producer = cfg.get<std::string>("BBoxProducer");


    _max_boxes = cfg.get<float>("MaxBoxes", 0);
    _unfilled_box_value = cfg.get<float>("UnfilledBoxValue", 0.0);
    _slice_v = cfg.get<std::vector<size_t> >("Channels", _slice_v);

    if (_max_boxes == 0){
      LARCV_CRITICAL() << "Maximum number of voxels must be non zero!" << std::endl;
      throw larbys();
    }

    if (dimension == 3 && _slice_v.size()) {
      LARCV_WARNING() << "BatchFillerBBox3D is only supported with one channel!" << std::endl;
    }


    LARCV_DEBUG() << "done" << std::endl;
  }

  template<size_t dimension>
  void BatchFillerBBox<dimension>::initialize() {}

  template<size_t dimension>
  void BatchFillerBBox<dimension>::_batch_begin_() {
    if(!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      LARCV_INFO() << "Batch size changed " << batch_data().dim().front() << "=>" << batch_size() << std::endl;
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }
  }

  template<size_t dimension>
  void BatchFillerBBox<dimension>::_batch_end_() {
    if (logger().level() <= msg::kINFO)
      LARCV_INFO() << "Total data size: " << batch_data().data_size()
                   << std::endl;
  }

  template<size_t dimension>
  void BatchFillerBBox<dimension>::finalize() { _entry_data.clear(); }

  template<size_t dimension>
  int BatchFillerBBox<dimension>::_check_projection(const size_t & projection_id) {

    if (dimension == 3) {
      return 0;
    }

    bool found = false;
    int count = 0;
    for (auto & channel : _slice_v){
      if (channel == projection_id){
        found = true;
        break;
      }
      count ++;
    }

    if (found) {
      return count;
    } else {
      return -1;
    }
  }



  template<size_t dimension>
  bool BatchFillerBBox<dimension>::process(IOManager& mgr) {

    /*
    Here we're collecting bboxes and buffering them into an array.
    Each array is of shape:
    [batch_size][num_channels][max_boxes][2*dimension]
    the [2] represents first the centroid, then the extents (half lengths)
    */


    LARCV_DEBUG() << "start" << std::endl;
    auto const& bbox_data =
        mgr.get_data<larcv3::EventBBox<dimension>>(_bbox_producer);
    // if (!_allow_empty && bbox_data.as_vector().empty()) {
    //   LARCV_CRITICAL()
    //       << "Could not locate non-empty voxel data w/ producer name "
    //       << _bbox_producer << std::endl;
    //   throw larbys();
    // }

    _num_channels = dimension == 2 ? _slice_v.size() : 1;

    std::vector<int> dim;
    dim.resize(4);
    dim.at(0) = batch_size();
    dim.at(1) = _num_channels;
    dim.at(2) = _max_boxes;
    dim.at(3) = 2*dimension;
    this->set_dim(dim);

    if (_entry_data.size() != batch_data().entry_data_size())
      _entry_data.resize(batch_data().entry_data_size(), 0.);


    // Reset all values to 0.0 (or whatever is specified)
    for (auto& v : _entry_data) v = _unfilled_box_value;

    // Assert that the requested channels are in the bbox vector:
    if (bbox_data.size() < _num_channels){
      LARCV_CRITICAL() << "Insufficient projections for BBox Filler " << std::endl;
      throw larbys();
    }




    for ( size_t _projection = 0; _projection < _num_channels; _projection ++ ){
      // Check that this projection ID is in the lists of channels:
      int count = _check_projection(_projection);
      if (count < 0) continue;

      auto & bbox_collection = bbox_data.as_vector().at(_projection);

      size_t max_bbox(bbox_collection.size());
      if (max_bbox > _max_boxes) {
        max_bbox = _max_boxes;
        LARCV_CRITICAL() << "Truncating the number of boxes to " << _max_boxes << "!" << std::endl;
      }


      size_t index(0);
      for (size_t i_bbox = 0; i_bbox < max_bbox; i_bbox ++) {


        // Index is the first index for this bbox.

        // So, it's _projection*projection_stride + i_box * box_stride
        // box_stride = 2*dimension
        // projection_stride = box_stride * max_boxes

        index = _projection * (2*dimension*_max_boxes) + i_bbox * (2*dimension);

        for (size_t i_dim = 0; i_dim < dimension; i_dim ++){
          _entry_data.at(index + 0         + i_dim) = bbox_collection.bbox(i_bbox).centroid()[i_dim];
          _entry_data.at(index + dimension + i_dim) = bbox_collection.bbox(i_bbox).half_length()[i_dim];

        }

      }

      // Only read the first voxel set in 3D
      if (dimension == 3) {
        break;
      }
    }

    // record the entry data
    LARCV_INFO() << "Inserting entry data of size " << _entry_data.size()
                 << std::endl;

    set_entry_data(_entry_data);

    return true;
  }

template class BatchFillerBBox<2>;
template class BatchFillerBBox<3>;
}
#endif
