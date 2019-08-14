#ifndef __LARCV3THREADIO_BATCHDATAQUEUE_CXX
#define __LARCV3THREADIO_BATCHDATAQUEUE_CXX

#include "BatchDataQueue.h"
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/base/larbys.h"

namespace larcv3 {






  template <class T>
  BatchDataQueue<T>::BatchDataQueue()
  {
  }

  template <class T>
  void BatchDataQueue<T>::reset()
  {
    
  }


  template <class T>
  bool BatchDataQueue<T>::is_next_ready () const
  {
    return _data_next.is_filled();
  }

  
  template <class T>
  const BatchData<T>& BatchDataQueue<T>::get_batch () const
  {
    return _data_current;
  }

  template <class T>
  BatchData<T>& BatchDataQueue<T>::get_next_writeable()
  {

    return _data_next;
  }

  template <class T>
  void BatchDataQueue<T>::set_next_data  (const std::vector<T>& source)
  {
    _data_next.set_entry_data(source);
  }

  template <class T>
  void BatchDataQueue<T>::pop(){
    _data_current = std::move(_data_next);
    _data_next.reset_data();
  }

}

template class larcv3::BatchDataQueue<short>;
template class larcv3::BatchDataQueue<int>;
template class larcv3::BatchDataQueue<float>;
template class larcv3::BatchDataQueue<double>;

#endif
