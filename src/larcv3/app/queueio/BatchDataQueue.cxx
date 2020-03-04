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
// template class larcv3::BatchDataQueue<larcv3::SparseTensor<2>>;


template<class T>
void init_batchdataqueue(pybind11::module m){

    using Class = larcv3::BatchDataQueue<T>;
    std::string classname = "BatchDataQueue" + larcv3::as_string<T>();
    pybind11::class_<Class> batch_data_queue(m, classname.c_str());
    batch_data_queue.def(pybind11::init<>());
    batch_data_queue.def("reset",          &Class::reset);
    batch_data_queue.def("next_state",     &Class::next_state);
    batch_data_queue.def("is_next_ready",  &Class::is_next_ready);
    batch_data_queue.def("get_batch",      &Class::get_batch);
    batch_data_queue.def("pop",            &Class::pop);


}

void init_batchdataqueue(pybind11::module m){
  init_batchdataqueue<short>(m);
  init_batchdataqueue<int>(m);
  init_batchdataqueue<float>(m);
  init_batchdataqueue<double>(m);

}

#endif
