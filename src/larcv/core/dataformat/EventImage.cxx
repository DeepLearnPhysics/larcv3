#ifndef EVENTIMAGE_CXX
#define EVENTIMAGE_CXX

#include "EventImage.h"
// #include "larcv/core/Base/larbys.h"

namespace larcv {

  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static EventImageFactory __global_EventImageFactory__;

  void EventImage::clear()
  {
    _image_v.clear();
  }

  const Image& EventImage::at(ImageIndex_t id) const
  {
    if( id >= _image_v.size() ) throw larbys("Invalid request (ImageIndex_t out-o-range)!");
    return _image_v[id];
  }

  void EventImage::append(const Image& img)
  {
    _image_v.push_back(img);
    _image_v.back().index((ImageIndex_t)(_image_v.size()-1));
  }

  void EventImage::emplace(Image&& img)
  {
    _image_v.emplace_back(std::move(img));
    _image_v.back().index((ImageIndex_t)(_image_v.size()-1));
  }

  void EventImage::emplace(std::vector<larcv::Image>&& image_v)
  {
    _image_v = std::move(image_v);
    for(size_t i=0; i<_image_v.size(); ++i) _image_v[i].index((ImageIndex_t)i);
  }


  void EventImage::initialize (H5::Group * group){
    return;
  }
  void EventImage::serialize  (H5::Group * group){
    return;
  }
  void EventImage::deserialize(H5::Group * group, size_t entry){
    return;
  }


}

#endif
