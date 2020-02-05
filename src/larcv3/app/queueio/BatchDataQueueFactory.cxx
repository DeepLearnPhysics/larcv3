#ifndef __LARCV3THREADIO_BATCHDATAQUEUEFACTORY_CXX
#define __LARCV3THREADIO_BATCHDATAQUEUEFACTORY_CXX

#include "BatchDataQueueFactory.h"

namespace larcv3 {



  template <class T>
  const BatchDataQueueFactory<T>& BatchDataQueueFactory<T>::get(){
    if (!_me) _me = new BatchDataQueueFactory<T>();
    return (*_me);
  }
    
  // Pop all queues to promote next to current
  template <class T>
  void BatchDataQueueFactory<T>::pop_all(){
    for (auto & batch_queue : _queue_m){
      batch_queue.second.pop();
    }
  }
  
  template <class T>
  bool BatchDataQueueFactory<T>::is_next_ready() const{
    bool ready = true;
    for (auto & batch_queue : _queue_m) ready = ready && batch_queue.second.is_next_ready();
    return ready;
  }

  template <class T>
  const BatchDataQueue<T>& BatchDataQueueFactory<T>::get_queue(std::string name) const{
    if (!_me) _me = new BatchDataQueueFactory<T>();
    auto iter = _queue_m.find(name);
    if (iter == _queue_m.end()) {
      LARCV_SCRITICAL() << "BatchDataQueue w/ name " << name << " not found!" << std::endl;
      throw larbys();
    }
    return iter->second;
  }




  template <class T>
  BatchDataQueue<T>& BatchDataQueueFactory<T>::get_queue_writeable(std::string name) {
    if (!_me) _me = new BatchDataQueueFactory<T>();
    auto iter = _queue_m.find(name);
    if (iter == _queue_m.end()) {
      LARCV_SCRITICAL() << "BatchDataQueue w/ name " << name << " not found!" << std::endl;
      throw larbys();
    }
    return iter->second;
  }

  template <class T>
  BatchDataQueueFactory<T>& BatchDataQueueFactory<T>::get_writeable() {
    if (!_me) _me = new BatchDataQueueFactory<T>();
    return (*_me);
  }
}
// template<> larcv3::BatchDataQueueFactory<char>*   larcv3::BatchDataQueueFactory<char>::_me   = nullptr;
template<> larcv3::BatchDataQueueFactory<short>*  larcv3::BatchDataQueueFactory<short>::_me  = nullptr;
template<> larcv3::BatchDataQueueFactory<int>*    larcv3::BatchDataQueueFactory<int>::_me    = nullptr;
template<> larcv3::BatchDataQueueFactory<float>*  larcv3::BatchDataQueueFactory<float>::_me  = nullptr;
template<> larcv3::BatchDataQueueFactory<double>* larcv3::BatchDataQueueFactory<double>::_me = nullptr;
// template<> larcv3::BatchDataQueueFactory<larcv3::SparseTensor<2>>* larcv3::BatchDataQueueFactory<larcv3::SparseTensor<2>>::_me = nullptr;
// template<> larcv3::BatchDataQueueFactory<std::string>* larcv3::BatchDataQueueFactory<std::string>::_me = nullptr;

// template class larcv3::BatchDataQueueFactory<char>;
template class larcv3::BatchDataQueueFactory<short>;
template class larcv3::BatchDataQueueFactory<int>;
template class larcv3::BatchDataQueueFactory<float>;
template class larcv3::BatchDataQueueFactory<double>;
// template class larcv3::BatchDataQueueFactory<larcv3::SparseTensor<2>>;
// template class larcv3::BatchDataQueueFactory<std::string>;

template<class T>
void init_batchdataqueuefactory(pybind11::module m){

    using Class = larcv3::BatchDataQueueFactory<T>;
    std::string classname = "BatchDataQueueFactory" + larcv3::as_string<T>();
    pybind11::class_<Class> batch_data_queue(m, classname.c_str());
    batch_data_queue.def(pybind11::init<>());
    batch_data_queue.def("get",              &Class::get);
    batch_data_queue.def("exist_queue",      &Class::exist_queue);
    batch_data_queue.def("is_next_ready",    &Class::is_next_ready);
    batch_data_queue.def("get_queue",        &Class::get_queue);
    batch_data_queue.def("pop_all",          &Class::pop_all);
    batch_data_queue.def("make_queue",       &Class::make_queue);

}

void init_batchdataqueuefactory(pybind11::module m){
  init_batchdataqueuefactory<short>(m);
  init_batchdataqueuefactory<int>(m);
  init_batchdataqueuefactory<float>(m);
  init_batchdataqueuefactory<double>(m);
  // init_batchdataqueuefactory<larcv3::SparseTensor<2>>(m);

}


#endif
