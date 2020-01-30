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


  // // Return a numpy array of this object (no copy by default)
  // template<size_t dimension>
  //  Tensor<dimension>::as_array(){
  //   // Cast the dimensions to std::array:
  //   std::array<size_t, dimension> dimensions;
  //   for (short i = 0; i < dimension; ++i) dimensions[i] = _meta.number_of_voxels(i);
  //   return pybind11::array_t<float>(
  //       dimensions,
  //       {},
  //       &(_img[0])
  //     );
  // }

  pybind11::array_t<float> VoxelSet::values() const {
    std::vector<float> ret;
    ret.resize(_voxel_v.size());
    size_t i = 0;
    for (auto & vox : _voxel_v){
      ret[i] = vox.value();
      i += 1;
    }
    return pybind11::array_t<float>(
        {ret.size()},
        {},
        &(ret[0])
      );
  }

  pybind11::array_t<size_t> VoxelSet::indexes() const {
    std::vector<size_t> ret;
    ret.resize(_voxel_v.size());
    size_t i = 0;
    for (auto & vox : _voxel_v){
      ret[i] = vox.id();
      i += 1;
    }
    return pybind11::array_t<size_t>(
        {ret.size()},
        {},
        &(ret[0])
      );
  }


  // std::vector<float> VoxelSet::values() const {
  //   std::vector<float> ret;
  //   ret.resize(_voxel_v.size());
  //   size_t i = 0;
  //   for (auto & vox : _voxel_v){
  //     ret[i] = vox.value();
  //     i += 1;
  //   }
  //   return ret;
  // }

  // std::vector<size_t> VoxelSet::indexes() const {
  //   std::vector<size_t> ret;
  //   ret.resize(_voxel_v.size());
  //   size_t i = 0;
  //   for (auto & vox : _voxel_v){
  //     ret[i] = vox.id();
  //     i += 1;
  //   }
  //   return ret;
  // }



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

#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

PYBIND11_MAKE_OPAQUE(std::vector<larcv3::Voxel>);
PYBIND11_MAKE_OPAQUE(std::vector<larcv3::VoxelSet>);

void init_voxel_core(pybind11::module m){
    using V   = larcv3::Voxel;
    using VS  = larcv3::VoxelSet;
    using VSA = larcv3::VoxelSetArray;

    pybind11::bind_vector<std::vector<V>>(m, "VectorOfVoxel");
    pybind11::bind_vector<std::vector<VS>>(m, "VectorOfVoxelSet");

    pybind11::class_<V> voxel(m, "Voxel");
    voxel.def(pybind11::init<larcv3::VoxelID_t, float>(),
      pybind11::arg("id")    = larcv3::kINVALID_VOXELID,
      pybind11::arg("value") = larcv3::kINVALID_FLOAT);
    voxel.def("id",    &V::id);
    voxel.def("value", &V::value);
    voxel.def("set",   &V::set);
    voxel.def(pybind11::self += float());
    voxel.def(pybind11::self -= float());
    voxel.def(pybind11::self *= float());
    voxel.def(pybind11::self /= float());
    voxel.def(pybind11::self == pybind11::self);
    voxel.def(pybind11::self <  pybind11::self);
    voxel.def(pybind11::self <= pybind11::self);
    voxel.def(pybind11::self >  pybind11::self);
    voxel.def(pybind11::self >= pybind11::self);

    voxel.def(pybind11::self == float());
    voxel.def(pybind11::self <  float());
    voxel.def(pybind11::self <= float());
    voxel.def(pybind11::self >  float());
    voxel.def(pybind11::self >= float());

    pybind11::class_<VS> voxelset(m, "VoxelSet");
    voxelset.def(pybind11::init<>());

    voxelset.def("id",             (larcv3::InstanceID_t (VS::*)() const)(&VS::id));
    voxelset.def("id",             (void (VS::*)(const larcv3::InstanceID_t))(&VS::id));
    voxelset.def("as_vector",      &VS::as_vector);
    voxelset.def("find",           &VS::find);
    voxelset.def("sum",            &VS::sum);
    voxelset.def("mean",           &VS::mean);
    voxelset.def("max",            &VS::max);
    voxelset.def("min",            &VS::min);
    voxelset.def("size",           &VS::size);
    voxelset.def("values",         &VS::values);
    voxelset.def("indexes",        &VS::indexes);
    voxelset.def("clear_data",     &VS::clear_data);
    voxelset.def("reserve",        &VS::reserve);
    voxelset.def("threshold",      &VS::threshold);
    voxelset.def("threshold_min",  &VS::threshold_min);
    voxelset.def("threshold_max",  &VS::threshold_max);
    voxelset.def("add",            &VS::add);
    voxelset.def("insert",         &VS::insert);
    voxelset.def("emplace",        (void (VS::*)(larcv3::VoxelID_t, float, const bool))(&VS::emplace));


    voxelset.def(pybind11::self += float());
    voxelset.def(pybind11::self -= float());
    voxelset.def(pybind11::self *= float());
    voxelset.def(pybind11::self /= float());

    /// Voxel Set Array

    pybind11::class_<VSA> voxelsetarray(m, "VoxelSetArray");
    voxelsetarray.def(pybind11::init<>());
    voxelsetarray.def("size",                 &VSA::size);
    voxelsetarray.def("voxel_set",            &VSA::voxel_set);
    voxelsetarray.def("as_vector",            &VSA::as_vector);
    voxelsetarray.def("sum",                  &VSA::sum);
    voxelsetarray.def("mean",                 &VSA::mean);
    voxelsetarray.def("max",                  &VSA::max);
    voxelsetarray.def("min",                  &VSA::min);
    voxelsetarray.def("clear_data",           &VSA::clear_data);
    voxelsetarray.def("resize",               &VSA::resize);
    voxelsetarray.def("threshold",            &VSA::threshold);
    voxelsetarray.def("threshold_min",        &VSA::threshold_min);
    voxelsetarray.def("threshold_max",        &VSA::threshold_max);
    voxelsetarray.def("writeable_voxel_set",  &VSA::writeable_voxel_set);
    voxelsetarray.def("insert",               &VSA::insert);

/*
  Not wrapped:
    void emplace(std::vector<larcv3::VoxelSet>&& voxel_vv);
    void emplace(larcv3::VoxelSet&& voxel_v);
    void move(larcv3::VoxelSetArray&& orig)
*/

}

template<size_t dimension>
void init_sparse_tensor(pybind11::module m){

    using ST = larcv3::SparseTensor<dimension>;
    std::string classname = "SparseTensor" + std::to_string(dimension) + "D";
    pybind11::class_<ST, larcv3::VoxelSet> sparsetensor(m, classname.c_str());
    sparsetensor.def(pybind11::init<>());
    sparsetensor.def("meta", (const larcv3::ImageMeta<dimension>& (ST::*)() const )(&ST::meta));
    sparsetensor.def("meta", (void (ST::*)(const larcv3::ImageMeta<dimension>& )  )(&ST::meta));
    sparsetensor.def("emplace", (void (ST::*)(const larcv3::Voxel &, const bool))(&ST::emplace));
    sparsetensor.def("set", &ST::set);
    sparsetensor.def("clear_data", &ST::clear_data);

/*
  Not wrapped:
    SparseTensor(VoxelSet&& vs, ImageMeta<dimension> meta);
    SparseTensor& operator= (const VoxelSet& rhs)
    inline void emplace(VoxelSet&& vs, const ImageMeta<dimension>& meta)

*/

}

template<size_t dimension>
void init_sparse_cluster(pybind11::module m){

    using SC = larcv3::SparseCluster<dimension>;
    std::string classname = "SparseCluster" + std::to_string(dimension) + "D";
    pybind11::class_<SC, larcv3::VoxelSetArray> sparsecluster(m, classname.c_str());
    sparsecluster.def(pybind11::init<>());
    sparsecluster.def("meta", (const larcv3::ImageMeta<dimension>& (SC::*)() const )(&SC::meta));
    sparsecluster.def("meta", (void (SC::*)(const larcv3::ImageMeta<dimension>& )  )(&SC::meta));
    sparsecluster.def("clear_data", &SC::clear_data);

/*
   Not wrapped:
    SparseCluster(VoxelSetArray&& vsa, ImageMeta<dimension> meta);

    inline void set(VoxelSetArray&& vsa, const ImageMeta<dimension>& meta)
*/

}

void init_voxel(pybind11::module m){
  init_voxel_core(m);
  init_sparse_tensor<2>(m);
  init_sparse_tensor<3>(m);
  init_sparse_cluster<2>(m);
  init_sparse_cluster<3>(m);
}




#endif
