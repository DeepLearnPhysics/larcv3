#ifndef __LARCV3THREADIO_BATCHFILLERSPARSETENSOR_CXX__
#define __LARCV3THREADIO_BATCHFILLERSPARSETENSOR_CXX__

#include "BatchFillerSparseTensor.h"

#include <random>

namespace larcv3 {

  /// Global larcv3::BatchFillerSparseTensorProcessFactory to register BatchFillerSparseTensor
  static BatchFillerSparseTensorProcessFactory<2>
      __global_BatchFillerSparseTensor2DProcessFactory__;
  static BatchFillerSparseTensorProcessFactory<3>
      __global_BatchFillerSparseTensor3DProcessFactory__;

  template<size_t dimension>
  BatchFillerSparseTensor<dimension>::BatchFillerSparseTensor(const std::string name)
      : BatchFillerTemplate<float>(name) {}

  template<size_t dimension>
  void BatchFillerSparseTensor<dimension>::configure(const PSet& cfg) {
    LARCV_DEBUG() << "start" << std::endl;
    _tensor_producer = cfg.get<std::string>("TensorProducer");
    _augment = cfg.get<bool>("Augment", true);

    // Max voxels imposes a limit to make the memory layout regular.  Assuming average sparsity of x% , it's safe to
    // set _max_voxels to n_rows*n_cols*x*2 or so.  It's still a dramatic memory reduction.
    _max_voxels = cfg.get<float>("MaxVoxels", 0);
    _unfilled_voxel_value = cfg.get<float>("UnfilledVoxelValue", 0.0);
    _slice_v = cfg.get<std::vector<size_t> >("Channels", _slice_v);
    _include_values = cfg.get<bool>("IncludeValues", true);

    if (_max_voxels == 0){
      LARCV_CRITICAL() << "Maximum number of voxels must be non zero!" << std::endl;
      throw larbys();
    }

    if (dimension == 3 && _slice_v.size() != 1) {
      LARCV_CRITICAL() << "BatchFillerSparseTensor3D is only supported with one channel!" << std::endl;
      throw larbys();
    }

    _allow_empty = cfg.get<bool>("AllowEmpty",false);

    LARCV_DEBUG() << "done" << std::endl;
  }

  template<size_t dimension>
  void BatchFillerSparseTensor<dimension>::initialize() {}

  template<size_t dimension>
  void BatchFillerSparseTensor<dimension>::_batch_begin_() {
    if(!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      LARCV_INFO() << "Batch size changed " << batch_data().dim().front() << "=>" << batch_size() << std::endl;
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }
  }

  template<size_t dimension>
  void BatchFillerSparseTensor<dimension>::_batch_end_() {
    if (logger().level() <= msg::kINFO)
      LARCV_INFO() << "Total data size: " << batch_data().data_size()
                   << std::endl;
  }

  template<size_t dimension>
  void BatchFillerSparseTensor<dimension>::finalize() { _entry_data.clear(); }

  template<size_t dimension>
  int BatchFillerSparseTensor<dimension>::_check_projection(const int & projection_id) {

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
  bool BatchFillerSparseTensor<dimension>::process(IOManager& mgr) {

    /*

    Filling a dense tensor of dimensions B, H, W, C produces, in the end,
    a 4D tensor.  Here, we want to produce a list of points in sparse format.
    SO, this will be a 3 dimensional tensor of dimensions B, N_max, dims_len
    Where dims_len is the number of values that represent a point.
    In this 2D example, dims_len == 3 (H, W, Value) which are extracted from
    the original image.

    In principle this can be bigger or smaller for different sized inputs:
    dims_len = 4 (H, W, D, Value) in 3D, for example

    The N_max value is a limit on how much memory is allocated for each event.
    The empty values are all set to 0.0, which are easily ignored in the
    graph networks since the pooling layers are typically max reductions

    By convention, the Value component is always last in the network

    If there are more points in the input than are available in the output,
    The points are truncated.

    It's possible to do a random downsampling, but in case of segementation
    networks this needs to be coordinated across the image and label filler.

    */


    LARCV_DEBUG() << "start" << std::endl;
    auto const& voxel_data =
        mgr.get_data<larcv3::EventSparseTensor<dimension>>(_tensor_producer);
    if (!_allow_empty && voxel_data.as_vector().empty()) {
      LARCV_CRITICAL()
          << "Could not locate non-empty voxel data w/ producer name "
          << _tensor_producer << std::endl;
      throw larbys();
    }

    _num_channels = _slice_v.size();

    int point_dim = dimension + 1;
    if (!_include_values){
      point_dim = dimension;
    }

    std::vector<int> dim;
    dim.resize(4);
    dim.at(0) = batch_size();
    dim.at(1) = _num_channels;
    dim.at(2) = _max_voxels;
    dim.at(3) = point_dim;
    this->set_dim(dim);


    auto const& voxel_meta = voxel_data.as_vector().front().meta();
    std::vector<int> dense_dim;
    dense_dim.resize(dimension + 2);
    dense_dim[0] = batch_size();
    for (size_t i = 1; i < dimension + 1; i++) {
      dense_dim[i] = voxel_meta.number_of_voxels(i);
    }
    dense_dim[dimension + 1] = _num_channels;
    this->set_dense_dim(dense_dim);


    if (_entry_data.size() != batch_data().entry_data_size())
      _entry_data.resize(batch_data().entry_data_size(), 0.);


    // Reset all values to 0.0 (or whatever is specified)
    for (auto& v : _entry_data) v = _unfilled_voxel_value;

    // Get the random x/y/z flipping
    bool flip_x = false;
    bool flip_y = false;
    bool flip_z = false;
    if (_augment){
      flip_x = bool(rand() % 2);
      flip_y = bool(rand() % 2);
      flip_z = bool(rand() % 2);
    }


    for ( auto const& voxel_set : voxel_data.as_vector()){
      auto & meta = voxel_set.meta();
      auto projection_id = meta.id();

      // Check that this projection ID is in the lists of channels:
      int count = _check_projection(projection_id);
      if (count < 0) continue;

      size_t max_voxel(voxel_set.size());
      if (max_voxel > _max_voxels) {
        max_voxel = _max_voxels;
        LARCV_INFO() << "Truncating the number of voxels to " << _max_voxels << "!" << std::endl;
      }


      auto const& voxels = voxel_set.as_vector();
      int i_x, i_y, i_z;
      size_t index;

      for (size_t i_voxel = 0; i_voxel < max_voxel; i_voxel ++) {

        auto coords = meta.coordinates(voxels[i_voxel].id());

        i_x = coords.at(0);
        i_y = coords.at(1);
        if (dimension == 3) i_z = coords.at(2);

        if (flip_x) i_x = meta.number_of_voxels(0) - (i_x + 1);
        if (flip_y) i_y = meta.number_of_voxels(1) - (i_y + 1);
        if (flip_z && dimension == 3) i_z = meta.number_of_voxels(2) - (i_z + 1);

        index = count * (_max_voxels * point_dim) + i_voxel * point_dim;

        _entry_data.at(index + 0) = i_x;
        _entry_data.at(index + 1) = i_y;
        if (dimension == 3) _entry_data.at(index + 2) = i_z;

        if(_include_values) {
          _entry_data.at(index + dimension) = voxels[i_voxel].value();
        }
      }
    }

    // record the entry data
    LARCV_INFO() << "Inserting entry data of size " << _entry_data.size()
                 << std::endl;

    set_entry_data(_entry_data);

    return true;
  }
}
#endif
