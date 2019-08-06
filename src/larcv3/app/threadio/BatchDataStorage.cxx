#ifndef __LARCV3THREADIO_BATCHDATASTORAGE_CXX
#define __LARCV3THREADIO_BATCHDATASTORAGE_CXX

#include "BatchDataStorage.h"
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/base/larbys.h"

namespace larcv3 {

  template <class T>
  BatchDataStorage<T>::BatchDataStorage(size_t num_batch)
  { reset(num_batch); }

  template <class T>
  void BatchDataStorage<T>::reset(size_t num_batch)
  {
    if(!num_batch) {
      LARCV_SERROR() << "Cannot reset w/ num_batch==0!" << std::endl;
      return;
    }
    _data_v.resize(num_batch);
  }

  template <class T>
  int BatchDataStorage<T>::is_any_batch_ready () const
  {
    for(size_t id=0; id<_data_v.size(); ++id) {
      if(is_batch_ready(id)) return (int)id;
    }
    return -1;
  }

  template <class T>
  bool BatchDataStorage<T>::is_batch_ready (size_t batch_id) const
  {
    auto const& batch = get_batch(batch_id);
    return batch.is_filled();
  }

  template <class T>
  const BatchData<T>& BatchDataStorage<T>::get_any_batch  () const
  {
    int id = is_any_batch_ready();
    if(id<0) {
      LARCV_SCRITICAL() << "No batch is ready yet!" << std::endl;
      throw larbys();
    }
    return _data_v[id];
  }
  
  template <class T>
  const BatchData<T>& BatchDataStorage<T>::get_batch      (size_t batch_id) const
  {
    if(batch_id > _data_v.size()) {
      LARCV_SCRITICAL () << "Batch ID (" << batch_id << ")"
		      << " is invalid (must be < " << _data_v.size() << ")" << std::endl;
      throw larbys();
    }
    return _data_v[batch_id];
  }

  template <class T>
  BatchData<T>& BatchDataStorage<T>::get_batch_writeable(size_t batch_id)
  {
    if(batch_id > _data_v.size()) {
      LARCV_SERROR () << "Batch ID (" << batch_id << ")"
		      << " is invalid (must be < " << _data_v.size() << ")" << std::endl;
      throw larbys();
    }
    return _data_v[batch_id];
  }

  template <class T>
  void BatchDataStorage<T>::set_entry_data  (const std::vector<T>& source, size_t batch_id)
  {
    auto& batch = get_batch_writeable(batch_id);
    batch.set_entry_data(source);
  }

  template <class T>
  void BatchDataStorage<T>::reset_batch(size_t batch_id, std::vector<int> dim)
  {
    auto& data = get_batch_writeable(batch_id);
    data.reset();
    data.set_dim(dim);
  }

  template <class T>
  void BatchDataStorage<T>::reset_batch(size_t batch_id)
  {
    auto& data = get_batch_writeable(batch_id);
    data.reset_data();
  }

  template <class T>
  void BatchDataStorage<T>::reset_batch()
  { for(size_t id=0; id<num_batch(); ++id) reset_batch(id); }

}

// template class larcv3::BatchDataStorage<char>;
template class larcv3::BatchDataStorage<short>;
template class larcv3::BatchDataStorage<int>;
template class larcv3::BatchDataStorage<float>;
template class larcv3::BatchDataStorage<double>;
// template class larcv3::BatchDataStorage<std::string>;

#endif
