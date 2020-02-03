#ifndef __LARCV3THREADIO_BATCHFILLERSPARSETENSOR2D_CXX__
#define __LARCV3THREADIO_BATCHFILLERSPARSETENSOR2D_CXX__

#include "BatchFillerSparseTensor2D.h"

#include <random>

namespace larcv3 {

static BatchFillerSparseTensor2DProcessFactory
    __global_BatchFillerSparseTensor2DProcessFactory__;

BatchFillerSparseTensor2D::BatchFillerSparseTensor2D(const std::string name)
    : BatchFillerTemplate<float>(name) {}

void BatchFillerSparseTensor2D::configure(const PSet& cfg) {
  LARCV_DEBUG() << "start" << std::endl;
  _tensor2d_producer = cfg.get<std::string>("Tensor2DProducer");
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

  _allow_empty = cfg.get<bool>("AllowEmpty",false);

  LARCV_DEBUG() << "done" << std::endl;
}

void BatchFillerSparseTensor2D::initialize() {}

void BatchFillerSparseTensor2D::_batch_begin_() {
  if(!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
    LARCV_INFO() << "Batch size changed " << batch_data().dim().front() << "=>" << batch_size() << std::endl;
    auto dim = batch_data().dim();
    dim[0] = batch_size();
    this->set_dim(dim);
  }
}

void BatchFillerSparseTensor2D::_batch_end_() {
  if (logger().level() <= msg::kINFO)
    LARCV_INFO() << "Total data size: " << batch_data().data_size()
                 << std::endl;
}

void BatchFillerSparseTensor2D::finalize() { _entry_data.clear(); }


bool BatchFillerSparseTensor2D::process(IOManager& mgr) {

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
      mgr.get_data<larcv3::EventSparseTensor2D>(_tensor2d_producer);
  if (!_allow_empty && voxel_data.as_vector().empty()) {
    LARCV_CRITICAL()
        << "Could not locate non-empty voxel data w/ producer name "
        << _tensor2d_producer << std::endl;
    throw larbys();
  }

  _num_channels = _slice_v.size();


  // if (_num_channels != voxel_data.as_vector().size()){
  //   LARCV_CRITICAL() << "Number of requested channels does not match number of available channels." << std::endl;
  //   throw larbys();
  // }

  int point_dim = 3; // This is hardcoded to be row, col, value, but could be extended
  if (!_include_values){
    point_dim = 2;
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
  dense_dim.resize(4);
  dense_dim[0] = batch_size();
  dense_dim[1] = voxel_meta.number_of_voxels(0);
  dense_dim[2] = voxel_meta.number_of_voxels(1);
  dense_dim[3] = _num_channels;
  this->set_dense_dim(dense_dim);


  if (_entry_data.size() != batch_data().entry_data_size())
    _entry_data.resize(batch_data().entry_data_size(), 0.);


  // Reset all values to 0.0 (or whatever is specified)
  for (auto& v : _entry_data) v = _unfilled_voxel_value;

  // Get the random x/y/z flipping
  bool flip_cols = false;
  bool flip_rows = false;
  if (_augment){
    flip_cols = bool(rand() % 2);
    flip_rows = bool(rand() % 2);
  }



  for ( auto const& voxel_set : voxel_data.as_vector()){
    auto & meta = voxel_set.meta();
    auto projection_id = meta.id();

    // Check that this projection ID is in the lists of channels:
    bool found = false;
    int count = 0;
    for (auto & channel : _slice_v){
      if (channel == projection_id){
        found = true;
        break;
      }
      count ++;
    }
    if (!found) continue;
    size_t max_voxel(voxel_set.size());
    if (max_voxel > _max_voxels) {
      max_voxel = _max_voxels;
      LARCV_INFO() << "Truncating the number of voxels!" << std::endl;
    }

    size_t index;
    int row(0), col(0);
    int row_mult(1), row_add(0);
    int col_mult(1), col_add(0);

    if (flip_rows){
      row_mult = -1;
      row_add = meta.rows() - 1;
    }
    if (flip_cols){
      col_mult = -1;
      col_add = meta.cols() - 1;
    }

    // Get all of the indexes:
    std::vector<size_t> indexes = voxel_set.indexes_vec();

    // Convert them all to coordinates:
    std::vector<size_t> coordinates;
    meta.coordinates(indexes, coordinates);


    if (_include_values){
      std::vector<float>  values  = voxel_set.values_vec();
#ifdef LARCV_OPENMP
      #pragma omp parallel private(row, col, index) shared(_entry_data, row_mult, row_add, col_mult, col_add)
#endif
      for (size_t i_voxel = 0; i_voxel < max_voxel; i_voxel ++) {
      // for (auto const& voxel : voxel_set.as_vector()) {
        row = coordinates[i_voxel*2];
        col = coordinates[i_voxel*2 + 1];

        col = col_mult*col + col_add;
        row = row_mult*row + row_add;


        index = count*(_max_voxels*point_dim) + i_voxel*point_dim;
        _entry_data.at(index) = row;
        _entry_data.at(index + 1) = col;
        _entry_data.at(index + 2) = values[i_voxel];
      }
    }
    else{
#ifdef LARCV_OPENMP
      #pragma omp parallel private(row, col, index) shared(_entry_data, row_mult, row_add, col_mult, col_add)
#endif
      for (size_t i_voxel = 0; i_voxel < max_voxel; i_voxel ++) {
      // for (auto const& voxel : voxel_set.as_vector()) {
        row = coordinates[i_voxel*2];
        col = coordinates[i_voxel*2 + 1];

        col = col_mult*col + col_add;
        row = row_mult*row + row_add;

        index = count*(_max_voxels*point_dim) + i_voxel*point_dim;
        _entry_data.at(index) = row;
        _entry_data.at(index + 1) = col;
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
