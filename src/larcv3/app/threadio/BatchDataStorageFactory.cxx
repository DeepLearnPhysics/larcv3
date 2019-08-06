#ifndef __LARCV3THREADIO_BATCHDATASTORAGEFACTORY_CXX
#define __LARCV3THREADIO_BATCHDATASTORAGEFACTORY_CXX

#include "BatchDataStorageFactory.h"

namespace larcv3 {

  template <class T>
  const BatchDataStorageFactory<T>& BatchDataStorageFactory<T>::get() {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    return (*_me);
  }

  template <class T>
  const BatchDataStorage<T>& BatchDataStorageFactory<T>::get_storage(std::string name) const
  {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    auto iter = _storage_m.find(name);
    if (iter == _storage_m.end()) {
      LARCV_SCRITICAL() << "BatchDataStorage w/ name " << name << " not found!" << std::endl;
      throw larbys();
    }
    return iter->second;
  }

  template <class T>
  BatchDataStorage<T>& BatchDataStorageFactory<T>::get_storage_writeable(std::string name) {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    auto iter = _storage_m.find(name);
    if (iter == _storage_m.end()) {
      LARCV_SCRITICAL() << "BatchDataStorage w/ name " << name << " not found!" << std::endl;
      throw larbys();
    }
    return iter->second;
  }

  template <class T>
  BatchDataStorageFactory<T>& BatchDataStorageFactory<T>::get_writeable() {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    return (*_me);
  }
}
// template<> larcv3::BatchDataStorageFactory<char>*   larcv3::BatchDataStorageFactory<char>::_me   = nullptr;
template<> larcv3::BatchDataStorageFactory<short>*  larcv3::BatchDataStorageFactory<short>::_me  = nullptr;
template<> larcv3::BatchDataStorageFactory<int>*    larcv3::BatchDataStorageFactory<int>::_me    = nullptr;
template<> larcv3::BatchDataStorageFactory<float>*  larcv3::BatchDataStorageFactory<float>::_me  = nullptr;
template<> larcv3::BatchDataStorageFactory<double>* larcv3::BatchDataStorageFactory<double>::_me = nullptr;
// template<> larcv3::BatchDataStorageFactory<std::string>* larcv3::BatchDataStorageFactory<std::string>::_me = nullptr;

// template class larcv3::BatchDataStorageFactory<char>;
template class larcv3::BatchDataStorageFactory<short>;
template class larcv3::BatchDataStorageFactory<int>;
template class larcv3::BatchDataStorageFactory<float>;
template class larcv3::BatchDataStorageFactory<double>;
// template class larcv3::BatchDataStorageFactory<std::string>;

#endif
