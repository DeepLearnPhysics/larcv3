#ifndef __LARCV3DATAFORMAT_VOXEL_CXX__
#define __LARCV3DATAFORMAT_VOXEL_CXX__

#include "larcv3/core/dataformat/Voxel.h"
#include <iostream>

namespace larcv3 {

  Voxel::Voxel(VoxelID_t id, float value)
  { _id = id; _value = value; }

  float VoxelSet::max() const
  {
    float val = std::numeric_limits<float>::min();
    for(auto const& vox : _voxel_v) {
      if(vox.value() > val) val = vox.value();
    }
    return val;
  }

  float VoxelSet::min() const
  {
    float val = std::numeric_limits<float>::max();
    for(auto const& vox : _voxel_v) {
      if(vox.value() < val) val = vox.value();
    }
    return val;
  }

  void VoxelSet::threshold(float min, float max)
  {
    std::vector<larcv3::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() < min || vox.value() > max) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }
  
  void VoxelSet::threshold_min(float min)
  {
    std::vector<larcv3::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() < min) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }
  
  void VoxelSet::threshold_max(float max)
  {
    std::vector<larcv3::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() > max) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::add(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),true);
  }

  void VoxelSet::insert(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),false);
  }

  const Voxel& VoxelSet::find(VoxelID_t id) const
  {
    if(_voxel_v.empty() ||
       id < _voxel_v.front().id() ||
       id > _voxel_v.back().id())
      return larcv3::kINVALID_VOXEL::getInstance();

    Voxel vox(id,0.);
    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);
    if( (*iter).id() == id ) return (*iter);
    else {
      //std::cout << "Returning invalid voxel since lower_bound had an id " << (*iter).id() << std::endl;
      return larcv3::kINVALID_VOXEL::getInstance();
    }
  }

  void VoxelSet::emplace(Voxel&& vox, const bool add)
  {
    // In case it's empty or greater than the last one
    if (_voxel_v.empty() || _voxel_v.back() < vox) {
      _voxel_v.emplace_back(std::move(vox));
      return;
    }
    // In case it's smaller than the first one
    if (_voxel_v.front() > vox) {
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = 0; (idx + 1) < _voxel_v.size(); ++idx) {
        auto& element1 = _voxel_v[ _voxel_v.size() - (idx + 1) ];
        auto& element2 = _voxel_v[ _voxel_v.size() - (idx + 2) ];
        std::swap( element1, element2 );
      }
      return;
    }

    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);

    // Cannot be the end
    if ( iter == _voxel_v.end() ) {
      std::cerr << "VoxelSet sorting logic error!" << std::endl;
      throw larbys();
    }

    // If found, merge
    if ( vox.id() == (*iter).id() ) {
      if(add) (*iter) += vox.value();
      else (*iter).set(vox.id(),vox.value());
      return;
    }
    // Else insert @ appropriate place
    else {
      size_t target_ctr = _voxel_v.size() - (iter - _voxel_v.begin());
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = 0; idx < target_ctr; ++idx) {
        auto& element1 = _voxel_v[ _voxel_v.size() - (idx + 1) ];
        auto& element2 = _voxel_v[ _voxel_v.size() - (idx + 2) ];
        std::swap( element1, element2 );
      }
    }
    return;
  }

  std::vector<float> VoxelSet::values() const {
    std::vector<float> ret;
    ret.resize(_voxel_v.size());
    size_t i = 0;
    for (auto & vox : _voxel_v){
      ret[i] = vox.value();
      i += 1;
    }
    return ret;
  }

  std::vector<size_t> VoxelSet::indexes() const {
    std::vector<size_t> ret;
    ret.resize(_voxel_v.size());
    size_t i = 0;
    for (auto & vox : _voxel_v){
      ret[i] = vox.id();
      i += 1;
    }
    return ret;
  }



  //
  // VoxelSetArray
  //
  float VoxelSetArray::sum() const
  { float res=0.; for(auto const& vox_v : _voxel_vv) res+=vox_v.sum(); return res;}

  float VoxelSetArray::mean() const
  { 
    size_t vox_ctr = 0; 
    for(auto const& vox_v : _voxel_vv) vox_ctr += vox_v.size(); 
    return (vox_ctr<1 ? 0. : this->sum() / (float)vox_ctr);
  }

  float VoxelSetArray::max() const
  {
    float val = std::numeric_limits<float>::min();
    float ival = 0.;
    for(auto const& vox_v : _voxel_vv) {
      ival = vox_v.max();
      val = (val < ival ? ival : val);
    }
    return val;
  }

  float VoxelSetArray::min() const
  {
    float val = std::numeric_limits<float>::max();
    float ival = 0.;
    for(auto const& vox_v : _voxel_vv) {
      ival = vox_v.min();
      val = (val > ival ? ival : val);
    }
    return val;
  }

  const larcv3::VoxelSet& VoxelSetArray::voxel_set(InstanceID_t id) const
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw larbys();
    }
    return _voxel_vv[id];
  }

  void VoxelSetArray::emplace(std::vector<larcv3::VoxelSet>&& voxel_vv)
  {
    _voxel_vv = std::move(voxel_vv);
    for(size_t id=0; id<_voxel_vv.size(); ++id)
      _voxel_vv[id].id(id);
  }

  // inline void VoxelSetArray::insert(const std::vector<larcv3::VoxelSet>& voxel_vv)
  // {
  //   _voxel_vv = voxel_vv;
  //   for(size_t id=0; id<_voxel_vv.size(); ++id)
  //     _voxel_vv[id].id(id);
  // }

  void VoxelSetArray::emplace(larcv3::VoxelSet&& voxel_v)
  {
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv[id].id(id);
    }
    _voxel_vv[voxel_v.id()] = std::move(voxel_v);
  }

  void VoxelSetArray::insert(const larcv3::VoxelSet& voxel_v)
  {
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv.at(id).id(id);
    }
    _voxel_vv.at(voxel_v.id()) = voxel_v;
  }

  larcv3::VoxelSet& VoxelSetArray::writeable_voxel_set(const InstanceID_t id)
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw larbys();
    }
    return _voxel_vv[id];
  }



template<size_t dimension>
SparseTensor<dimension>::SparseTensor(VoxelSet&& vs, ImageMeta<dimension> meta)
: VoxelSet(std::move(vs))
{ this->meta(meta); }

template<size_t dimension>
void SparseTensor<dimension>::meta(const larcv3::ImageMeta<dimension>& meta)
{
for (auto const& vox : this->as_vector()) {
 if (vox.id() < meta.total_voxels()) continue;
 std::cerr << "VoxelSet contains ID " << vox.id()
           << " which cannot exists in ImageMeta with size " << meta.total_voxels()
           << std::endl;
 throw larbys();
}
_meta = meta;
}

template<size_t dimension>
void SparseTensor<dimension>::emplace(const larcv3::Voxel & vox, const bool add)
{
if (vox.id() != kINVALID_VOXELID) VoxelSet::emplace(vox.id(), vox.value(), add);
}

template<size_t dimension>
void SparseCluster<dimension>::meta(const larcv3::ImageMeta<dimension>& meta)
{
for (auto const& vs : this->as_vector()) {
 for (auto const& vox : vs.as_vector()) {
   if (vox.id() < meta.total_voxels()) continue;
   std::cerr << "VoxelSet contains ID " << vox.id()
             << " which cannot exists in ImageMeta with size " << meta.total_voxels()
             << std::endl;
   throw larbys();
 }
}
_meta = meta;
}

template<size_t dimension>
SparseCluster<dimension>::SparseCluster(VoxelSetArray&& vsa, ImageMeta<dimension> meta)
: VoxelSetArray(std::move(vsa))
{ this->meta(meta); }


// Instantiate the objects for 2D and 3D:
template class SparseTensor<2> ;
template class SparseTensor<3> ;
template class SparseCluster<2>;
template class SparseCluster<3>;

}



#endif
