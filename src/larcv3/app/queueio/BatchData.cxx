#ifndef __LARCV3THREADIO_BATCHDATA_CXX
#define __LARCV3THREADIO_BATCHDATA_CXX

#include "BatchData.h"
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/base/larbys.h"
#include <sstream>

namespace larcv3 {

  template<class T>
  const std::vector<T>& BatchData<T>::data() const
  {
    if (_state != BatchDataState_t::kBatchStateFilled) {
      LARCV_SCRITICAL() << "Current batch state: " << (int)_state
                        << " not ready to expose data!" << std::endl;
      throw larbys();
    }
    return _data;
  }

  template<class T>
  pybind11::array_t<T> BatchData<T>::pydata()
  {
    if (_state != BatchDataState_t::kBatchStateFilled) {
      LARCV_SCRITICAL() << "Current batch state: " << (int)_state
                        << " not ready to expose data!" << std::endl;
      throw larbys();
    }

    // First, create the buffer object:
    // Cast the dimensions to std::array:
    std::array<size_t, 1> dimensions;
    dimensions[0] = _data.size();
    // Allocate a spot to store the data:
    auto array = pybind11::array_t<T>(
        // _meta.number_of_voxels()[0],
        dimensions,
        {},
        &(_data[0])
      );
  
    return array;

  }

  template<class T>
  size_t BatchData<T>::data_size(bool calculate) const
  {
    if (_dim.empty()) return 0;
    if (!calculate && !_data.empty()) return _data.size();
    size_t length = 1;
    for (auto const& dim : _dim) length *= dim;
    return length;
  }

  template<class T>
  size_t BatchData<T>::entry_data_size() const
  {
    if (_dim.empty()) return 0;
    size_t length = 1;
    for (size_t i = 1; i < _dim.size(); ++i) length *= _dim[i];
    return length;
  }

  template<class T>
  void BatchData<T>::set_dim(const std::vector<int>& dim)
  {
    if (dim.empty()) {
      LARCV_SERROR() << "Dimension input has 0 length..." << std::endl;
      return;
    }
    if (_dim.size()) {

      if (_state != BatchDataState_t::kBatchStateEmpty &&
          _state != BatchDataState_t::kBatchStateReleased) {
        bool allowed = (_dim.size() == dim.size());
        if (allowed)
          for (size_t i = 0; i < _dim.size(); ++i) {allowed = allowed && (_dim[i] == dim[i]);}

        if (allowed) return;

        LARCV_SCRITICAL() << "Dimension cannot be re-set!" << std::endl;
        throw larbys();
      }
    }
    std::stringstream ss;
    ss << "Resetting the batch size: (" << dim.front();
    for (size_t i = 1; i < dim.size(); ++i) ss << "," << dim[i];
    ss << ")" << std::endl;
    LARCV_SINFO() << ss.str();
    _dim = dim;
    reset_data();
  }

  template<class T>
  void BatchData<T>::set_dense_dim(const std::vector<int>& dense_dim)
  {
    if (dense_dim.empty()) {
      LARCV_SERROR() << "Dimension input has 0 length..." << std::endl;
      return;
    }
    if (_dense_dim.size()) {

      if (_state != BatchDataState_t::kBatchStateEmpty &&
          _state != BatchDataState_t::kBatchStateReleased) {
        bool allowed = (_dense_dim.size() == dense_dim.size());
        if (allowed)
          for (size_t i = 0; i < _dense_dim.size(); ++i) {allowed = allowed && (_dense_dim[i] == dense_dim[i]);}

        if (allowed) return;

        LARCV_SCRITICAL() << "Dimension cannot be re-set!" << std::endl;
        throw larbys();
      }
    }
    std::stringstream ss;
    ss << "Resetting the batch size: (" << dense_dim.front();
    for (size_t i = 1; i < dense_dim.size(); ++i) ss << "," << dense_dim[i];
    ss << ")" << std::endl;
    LARCV_SINFO() << ss.str();
    _dense_dim = dense_dim;
    reset_data();
  }


  template<class T>
  void BatchData<T>::set_entry_data(const std::vector<T>& entry_data)
  {
    if (_state != BatchDataState_t::kBatchStateFilling &&
        _state != BatchDataState_t::kBatchStateEmpty) {
      LARCV_SERROR() << "Current batch state: " << (int)(_state)
                     << " not ready for filling data..." << std::endl;
      return;
    }
    _state = BatchDataState_t::kBatchStateFilling;

    size_t entry_size = entry_data_size();
    if ( (_current_size + entry_size) > data_size() ) {
      LARCV_SERROR() << "Current size (" << _current_size
                     << ") + entry data size (" << entry_size
                     << ") exceeds data buffer size (" << data_size()
                     << std::endl;
      return;
    }

    size_t entry_idx = 0;
    while (entry_idx < entry_data.size()) {
      _data[_current_size] = entry_data[entry_idx];
      ++entry_idx;
      ++_current_size;
    }
    // _data = std::move(entry_data);
    // _current_size = entry_data.size();
    if (_current_size == _data.size()){
      _state = BatchDataState_t::kBatchStateFilled;
    }
  }

  template <class T>
  void BatchData<T>::reset()
  {
    _data.clear(); _dim.clear();
    _current_size = 0;
    _state = BatchDataState_t::kBatchStateEmpty;
  }

  template <class T>
  void BatchData<T>::reset_data()
  {
    if (_state == BatchDataState_t::kBatchStateFilling) {
      LARCV_SERROR() << "Cannot reset batch (is in kBatchStateFilling state! size "
                     << _current_size << "/" << _data.size() << ")" << std::endl;
      return;
    }
    LARCV_SINFO() << "Resetting batch data status to " << (int)(BatchDataState_t::kBatchStateEmpty) << std::endl;
    _data.resize(data_size(true));
    _current_size = 0;
    _state = BatchDataState_t::kBatchStateEmpty;
  }

}

template class larcv3::BatchData<short>;
template class larcv3::BatchData<int>;
template class larcv3::BatchData<float>;
template class larcv3::BatchData<double>;

void init_batchdata(pybind11::module m){

  init_batchdata_<short>(m);
  init_batchdata_<int>(m);
  init_batchdata_<float>(m);
  init_batchdata_<double>(m);
  // init_batchdata_<larcv3::SparseTensor<2>>(m);

}

#include <typeinfo>
#include <pybind11/stl.h>

template <class T>
void init_batchdata_(pybind11::module m){

    using Class = larcv3::BatchData<T>;
    std::string classname = "BatchData" + larcv3::as_string<T>();
    pybind11::class_<Class> batch_data(m, classname.c_str());
    batch_data.def(pybind11::init<>());




    batch_data.def("pydata",             &Class::pydata);
    batch_data.def("data",               &Class::data);
    batch_data.def("dim",                &Class::dim);
    batch_data.def("dense_dim",          &Class::dense_dim);
    batch_data.def("data_size",          &Class::data_size,
      pybind11::arg("data_size")=false);
    batch_data.def("current_data_size",  &Class::current_data_size);
    batch_data.def("entry_data_size",    &Class::entry_data_size);
    batch_data.def("set_dim",            &Class::set_dim);
    batch_data.def("set_dense_dim",      &Class::set_dense_dim);
    batch_data.def("set_entry_data",     &Class::set_entry_data);
    batch_data.def("reset",              &Class::reset);
    batch_data.def("reset_data",         &Class::reset_data);
    batch_data.def("is_filled",          &Class::is_filled);
    batch_data.def("state",              &Class::state);

/*

    // PyObject * pydata() const;
    // PyObject * () const;
    // pybind11::array_t<float> pydata();

*/

}

#endif
