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
  void BatchFillerSparseTensor<dimension>::configure(const json& cfg) {
    LARCV_DEBUG() << "start" << std::endl;

    config = this -> default_config();
    config = augment_default_config(config, cfg);


    // _tensor_producer = cfg.get<std::string>("TensorProducer");
    // _augment = cfg.get<bool>("Augment", true);

    // Max voxels imposes a limit to make the memory layout regular.  Assuming average sparsity of x% , it's safe to
    // set _max_voxels to n_rows*n_cols*x*2 or so.  It's still a dramatic memory reduction.


    if (config["MaxVoxels"].template get<int>() == 0){
      LARCV_CRITICAL() << "Maximum number of voxels must be non zero!" << std::endl;
      throw larbys();
    }

    if (dimension == 3 && config["Channels"].template get<std::vector<int>>().size()) {
      LARCV_WARNING() << "BatchFillerSparseTensor3D is only supported with one channel!" << std::endl;
    }


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
  int BatchFillerSparseTensor<dimension>::_check_projection(const int & projection_id, const std::vector<int> & _slice_v) {

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

    auto _max_voxels            = config["MaxVoxels"].         template get<float>();
    auto _unfilled_voxel_value  = config["UnfilledVoxelValue"].template get<float>();
    auto _slice_v               = config["Channels"].          template get<std::vector<int> >();
    auto _include_values        = config["IncludeValues"].     template get<bool>();
    auto _tensor_producer       = config["TensorProducer"].    template get<std::string>();
    auto _augment               = config["Augment"].           template get<bool>();

    LARCV_DEBUG() << "start" << std::endl;
    auto const& voxel_data =
        mgr.get_data<larcv3::EventSparseTensor<dimension>>(_tensor_producer);
    if (!_allow_empty && voxel_data.as_vector().empty()) {
      LARCV_CRITICAL()
          << "Could not locate non-empty voxel data w/ producer name "
          << _tensor_producer << std::endl;
      throw larbys();
    }

    _num_channels = dimension == 2 ? _slice_v.size() : 1;
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
      dense_dim[i] = voxel_meta.number_of_voxels(i - 1);
    }
    dense_dim[dimension + 1] = _num_channels;
    this->set_dense_dim(dense_dim);


    if (_entry_data.size() != batch_data().entry_data_size())
      _entry_data.resize(batch_data().entry_data_size(), 0.);


    // Reset all values to 0.0 (or whatever is specified)
    for (auto& v : _entry_data) v = _unfilled_voxel_value;

    // Get the random x/y/z flipping
    std::vector<bool> flip;
    flip.resize(dimension);

    if (_augment){
      for (size_t i_d = 0; i_d < dimension; i_d ++)
        flip[i_d] = bool(rand() % 2);
    }


    for ( auto const& voxel_set : voxel_data.as_vector()){
      auto & meta = voxel_set.meta();
      auto projection_id = meta.id();

      // Check that this projection ID is in the lists of channels:
      int count = _check_projection(projection_id, _slice_v);
      if (count < 0) continue;

      size_t max_voxel(voxel_set.size());
      if (max_voxel > _max_voxels) {
        max_voxel = _max_voxels;
        LARCV_INFO() << "Truncating the number of voxels to " << _max_voxels << "!" << std::endl;
      }


      auto const& voxels = voxel_set.as_vector();
      std::vector<int> i_coord;
      i_coord.resize(dimension);
      size_t index;

      for (size_t i_voxel = 0; i_voxel < max_voxel; i_voxel ++) {

        auto coords = meta.coordinates(voxels[i_voxel].id());
        
        index = count * (_max_voxels * point_dim) + i_voxel * point_dim;

        for (size_t i_d = 0; i_d < dimension; i_d ++){
          i_coord[i_d] = coords.at(i_d);
          if (flip[i_d]){
            i_coord[i_d] = meta.number_of_voxels(i_d) - (i_coord[i_d] + 1);
          }
          _entry_data.at(index + i_d) = i_coord[i_d];
        }

        if(_include_values) {
          _entry_data.at(index + dimension) = voxels[i_voxel].value();
        }

        // i_x = coords.at(0);
        // i_y = coords.at(1);
        // if (dimension == 3) i_z = coords.at(2);

        // if (flip_x) i_x = meta.number_of_voxels(0) - (i_x + 1);
        // if (flip_y) i_y = meta.number_of_voxels(1) - (i_y + 1);
        // if (flip_z && dimension == 3) i_z = meta.number_of_voxels(2) - (i_z + 1);



        // _entry_data.at(index + 0) = i_x;
        // _entry_data.at(index + 1) = i_y;
        // if (dimension == 3) _entry_data.at(index + 2) = i_z;


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
}
#endif
