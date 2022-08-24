#pragma once

namespace larcv3{
    
    template<class dataproduct>
    EventBase<dataproduct>::~EventBase(){
        // for (auto & p : _data_types){
        //     delete p;
        // }
    }

    template<class dataproduct>
    int EventBase<dataproduct>::get_num_objects(hid_t group){
        hsize_t  num_objects[1] = {0};
        H5Gget_num_objs(group, num_objects);
        return num_objects[0];
    }


    template<class dataproduct>
    void EventBase<dataproduct>::set(const std::vector<dataproduct>& object_v)
    {
        _object_v = object_v;
        for(size_t i=0; i<_object_v.size(); ++i){
            _object_v[i].id(i);
        }
    }

  template <class dataproduct>
  void EventBase<dataproduct>::append(const dataproduct& obj)
  {
    _object_v.push_back(obj);
    _object_v.back().id(_object_v.size()-1);
  }

  template <class dataproduct>
  void EventBase<dataproduct>::emplace_back(dataproduct&& obj)
  {
    _object_v.emplace_back(std::move(obj));
    _object_v.back().id(_object_v.size()-1);
  }

  template <class dataproduct>
  void EventBase<dataproduct>::emplace(std::vector<dataproduct>&& object_v)
  {
    _object_v = std::move(object_v);
    for(size_t i=0; i<_object_v.size(); ++i)
      _object_v[i].id(i);
  }

  template <class dataproduct>
  void EventBase<dataproduct>::clear(){
    _object_v.clear();
  }



  template <class dataproduct>
  void EventBase<dataproduct>::finalize(){
    for (size_t i =0; i < _open_in_datasets.size(); i ++){
      H5Dclose(_open_in_datasets[i]);
    }
    for (size_t i =0; i < _open_out_datasets.size(); i ++){
      H5Dclose(_open_out_datasets[i]);
    }
  }

}



