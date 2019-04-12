#ifndef __LARCV_EVENTVOXEL_CXX
#define __LARCV_EVENTVOXEL_CXX

#include "EventVoxel.h"


namespace larcv {

  /// Global larcv::EventClusterPixel2DFactory to register EventSparseClusters
  // static EventSparseClustersFactory __global_EventSparseClustersFactory__;

  /// Global larcv::EventSparseTensorFactory to register EventSparseTensor
  static EventSparseTensor2DFactory __global_EventSparseTensor2DFactory__;
  static EventSparseTensor3DFactory __global_EventSparseTensor3DFactory__;

  
  // // EventSparseClusters
  // //
  // const larcv::SparseCluster&
  // EventSparseClusters::sparse_cluster(const ProjectionID_t id) const
  // {
  //   if(id >= _cluster_v.size()) {
  //     std::cerr << "EventSparseClusters does not hold any SparseCluster for ProjectionID_t " << id << std::endl;
  //     throw larbys();
  //   }
  //   return _cluster_v[id];
  // }

  // void EventSparseClusters::emplace(larcv::SparseCluster&& clusters)
  // {
  //   if(_cluster_v.size() <= clusters.meta().id())
  //     _cluster_v.resize(clusters.meta().id()+1);
  //   _cluster_v[clusters.meta().id()] = std::move(clusters);
  // }

  // void EventSparseClusters::set(const larcv::SparseCluster& clusters) 
  // {
  //   if(_cluster_v.size() <= clusters.meta().id())
  //     _cluster_v.resize(clusters.meta().id()+1);
  //   _cluster_v[clusters.meta().id()] = clusters;
    
  // }
  
  // void EventSparseClusters::emplace(larcv::VoxelSetArray&& clusters, larcv::ImageMeta<dimension>&& meta)
  // {
  //   larcv::SparseCluster source(std::move(clusters),std::move(meta));
  //   // source.set();
  //   emplace(std::move(source));
  // }

  // void EventSparseClusters::set(const larcv::VoxelSetArray& clusters, const larcv::ImageMeta<dimension>& meta)
  // {

  //   if(_cluster_v.size() <= clusters.meta().id())
  //     _cluster_v.resize(clusters.meta().id()+1);
  //   _cluster_v[clusters.meta().id()] = clusters;
    

  //   larcv::SparseCluster source;
  //   source.emplace(std::move(clusters), meta);
  //   // source.meta(meta);
  //   emplace(std::move(source));
  // }

  //
  // EventSparseTensor
  //
  template<size_t dimension> 
  const larcv::SparseTensor<dimension>&
  EventSparseTensor<dimension>::sparse_tensor(const ProjectionID_t id) const
  {
    if(id >= _tensor_v.size()) {
      std::cerr << "EventSparseTensor does not hold any SparseTensor for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _tensor_v[id];
  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::emplace(larcv::SparseTensor<dimension>&& voxels)
  {
    if(_tensor_v.size() <= voxels.meta().id())
      _tensor_v.resize(voxels.meta().id()+1);
    _tensor_v[voxels.meta().id()] = std::move(voxels);

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::set(const larcv::SparseTensor<dimension>& voxels) 
  {
    if(_tensor_v.size() <= voxels.meta().id())
      _tensor_v.resize(voxels.meta().id()+1);
    _tensor_v[voxels.meta().id()] = voxels;
  }
  
  template<size_t dimension> 
  void EventSparseTensor<dimension>::emplace(larcv::VoxelSet&& voxels, larcv::ImageMeta<dimension>&& meta)
  {
    larcv::SparseTensor<dimension> source(std::move(voxels),std::move(meta));
    emplace(std::move(source));
  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::set(const larcv::VoxelSet& voxels, const larcv::ImageMeta<dimension>& meta)
  {
    larcv::SparseTensor<dimension> source;
    source.set(voxels, meta);
    // source.meta(meta);
    emplace(std::move(source));
  }

  // IO functions: 
  template<size_t dimension> 
  void EventSparseTensor<dimension>::initialize (H5::Group * group){

    // Call the helper initialize function:
    _helper.initialize_voxel_group(group);


  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::serialize  (H5::Group * group){
    // TODO

    // Step one, we write the meta if it hasn't already been written.
    // The helper handles this, if we set the meta:

    if (! _helper.initialized()){
      _helper.image_meta.clear();
      for (auto & sparse_tensor : _tensor_v)
        _helper.image_meta.push_back(sparse_tensor.meta());

      _helper.initialize_for_write(group);

      
    }
    else{
      // In this case, we make sure that since the helper is initialized, we have 
      // the same meta going in to the file:
      for (size_t i = 0; i <  _tensor_v.size(); ++ i){
        if (_tensor_v.at(i).meta() != _helper.image_meta.at(i)){
          LARCV_CRITICAL() << "Meta has been changed since this group was first initialized!  ERROR." << std::endl;
          throw larbys();
        }
      }

    }
    
    // We unpack the voxels of the sparse tensor into one long list:
    /// TODO
    std::vector<std::vector<larcv::VoxelSet> > _voxels;

    _voxels.resize(_tensor_v.size());
    for (size_t projection_id = 0; projection_id < _tensor_v.size(); projection_id ++){
      _voxels.at(projection_id).resize(1);
      for (auto & voxel : _tensor_v.at(projection_id).as_vector()){
        _voxels.at(projection_id).front().insert(voxel);
      }
    }


    // User the helper to write them to file:
    _helper.write_voxels(group, _voxels);
    // Serialization relies on the helper to write everything.
    return;

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::deserialize(H5::Group * group, size_t entry){

    // Make sure to read in meta before writing:
    if (! _helper.initialized()){
      _helper.read_projection_meta(group);
      _helper.image_meta.clear();
      _helper.read_image_meta(group); 
      _helper.initialize_for_read(group);
    }

    std::vector<std::vector<larcv::VoxelSet> > _voxels;
    _helper.read_voxels(group, entry, _voxels);


    _tensor_v.resize(_voxels.size());

    // Now the _voxels object is filled, have to populate the sparse tensor:
    for (size_t projection_id = 0; projection_id < _voxels.size(); projection_id ++){
      _tensor_v.at(projection_id).emplace(std::move(_voxels.at(projection_id).at(0)), _helper.image_meta.at(projection_id));
    }


    return;

  }
/*
  // IO functions: 
  void EventSparseClusters::initialize (H5::Group * group){

    // Call the helper initialize function:
    _helper.initialize_voxel_group(group);


  }

  void EventSparseClusters::serialize  (H5::Group * group){
    // Step one, we write the meta if it hasn't already been written.
    // The helper handles this, if we set the meta:

    if (! _helper.initialized()){
      _helper.image_meta.clear();
      for (auto & sparse_cluster : _cluster_v)
        _helper.image_meta.push_back(sparse_cluster.meta());

      _helper.initialize_for_write(group);

      
    }
    else{
      // In this case, we make sure that since the helper is initialized, we have 
      // the same meta going in to the file:
      for (size_t i = 0; i <  _cluster_v.size(); ++ i){
        if (_cluster_v.at(i).meta() != _helper.image_meta.at(i)){
          LARCV_CRITICAL() << "Meta has been changed since this group was first initialized!  ERROR." << std::endl;
          throw larbys();
        }
      }

    }
    
    // We unpack the voxels of the sparse tensor into one long list:
    /// TODO
    std::vector<std::vector<larcv::VoxelSet> > _voxels;

    _voxels.resize(_cluster_v.size());
    for (size_t projection_id = 0; projection_id < _cluster_v.size(); projection_id ++){
      _voxels.at(projection_id).resize(_cluster_v.at(projection_id).size());
      
      for (size_t cluster_id = 0; cluster_id < _cluster_v.at(projection_id).size(); cluster_id ++){
        for (auto & voxel : _cluster_v.at(projection_id).voxel_set(cluster_id).as_vector()){
          _voxels.at(projection_id).at(cluster_id).insert(voxel);
        }
      }
    }


    // User the helper to write them to file:
    _helper.write_voxels(group, _voxels);
    // Serialization relies on the helper to write everything.
    return;

  }

  void EventSparseClusters::deserialize(H5::Group * group, size_t entry){

    // Make sure to read in meta before writing:
    if (! _helper.initialized()){
      _helper.read_projection_meta(group);
      _helper.image_meta.clear();
      _helper.read_image_meta(group); 
      _helper.initialize_for_read(group);
    }

    std::vector<std::vector<larcv::VoxelSet> > _voxels;
    _helper.read_voxels(group, entry, _voxels);


    _cluster_v.resize(_voxels.size());

    // Now the _voxels object is filled, have to populate the sparse tensor:
    for (size_t projection_id = 0; projection_id < _voxels.size(); projection_id ++){
      // Set the meta for this projection:
      _cluster_v.at(projection_id).meta(_helper.image_meta.at(projection_id));
      _cluster_v.at(projection_id).emplace(std::move(_voxels.at(projection_id)));
      // for (size_t cluster_id = 0; cluster_id < _voxels.at(projection_id).size(); cluster_id ++){
      //   // Write the voxels into the appropriate cluster
      //   auto & _this_cluster = _cluster_v.at(projection_id).emplace();
      //   for (auto & vox : _voxels.at(projection_id).at(cluster_id).as_vector()){
      //     _this_cluster.insert(vox);
      //   }
      // }
    }

    return;

  }
*/

template class EventSparseTensor<2>;
template class EventSparseTensor<3>;

}

#endif
