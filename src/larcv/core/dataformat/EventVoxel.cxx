#ifndef __LARCV_EVENTVOXEL_CXX
#define __LARCV_EVENTVOXEL_CXX

#include "EventVoxel.h"


namespace larcv {

  /// Global larcv::EventClusterPixel2DFactory to register EventSparseClusters
  static EventSparseClustersFactory __global_EventSparseClustersFactory__;

  /// Global larcv::EventSparseTensorFactory to register EventSparseTensor
  static EventSparseTensorFactory __global_EventSparseTensorFactory__;

  
  // EventSparseClusters
  //
  const larcv::SparseCluster&
  EventSparseClusters::sparse_cluster(const ProjectionID_t id) const
  {
    if(id >= _cluster_v.size()) {
      std::cerr << "EventSparseClusters does not hold any SparseCluster for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _cluster_v[id];
  }

  void EventSparseClusters::emplace(larcv::SparseCluster&& clusters)
  {
    if(_cluster_v.size() <= clusters.meta().id())
      _cluster_v.resize(clusters.meta().id()+1);
    _cluster_v[clusters.meta().id()] = std::move(clusters);
  }

  void EventSparseClusters::set(const larcv::SparseCluster& clusters) 
  {
    if(_cluster_v.size() <= clusters.meta().id())
      _cluster_v.resize(clusters.meta().id()+1);
    _cluster_v[clusters.meta().id()] = clusters;
    
  }
  
  void EventSparseClusters::emplace(larcv::VoxelSetArray&& clusters, larcv::ImageMeta&& meta)
  {
    larcv::SparseCluster source(std::move(clusters),std::move(meta));
    // source.set();
    emplace(std::move(source));
  }

  // void EventSparseClusters::set(const larcv::VoxelSetArray& clusters, const larcv::ImageMeta& meta)
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
  const larcv::SparseTensor&
  EventSparseTensor::sparse_tensor(const ProjectionID_t id) const
  {
    if(id >= _tensor_v.size()) {
      std::cerr << "EventSparseTensor does not hold any SparseTensor for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _tensor_v[id];
  }

  void EventSparseTensor::emplace(larcv::SparseTensor&& voxels)
  {
    LARCV_CRITICAL() << "Id is : " << voxels.meta().id() << std::endl;
    if(_tensor_v.size() <= voxels.meta().id())
      _tensor_v.resize(voxels.meta().id()+1);
    _tensor_v[voxels.meta().id()] = std::move(voxels);
  }

  void EventSparseTensor::set(const larcv::SparseTensor& voxels) 
  {
    if(_tensor_v.size() <= voxels.meta().id())
      _tensor_v.resize(voxels.meta().id()+1);
    _tensor_v[voxels.meta().id()] = voxels;
  }
  
  void EventSparseTensor::emplace(larcv::VoxelSet&& voxels, larcv::ImageMeta&& meta)
  {
    larcv::SparseTensor source(std::move(voxels),std::move(meta));
    emplace(std::move(source));
  }

  void EventSparseTensor::set(const larcv::VoxelSet& voxels, const larcv::ImageMeta& meta)
  {
    larcv::SparseTensor source;
    source.set(voxels, meta);
    // source.meta(meta);
    emplace(std::move(source));
  }

  // IO functions: 
  void EventSparseTensor::initialize (H5::Group * group){

    // Call the helper initialize function:
    _helper.initialize_voxel_group(group);

    _helper.write_projection_meta(group);

  }

  void EventSparseTensor::serialize  (H5::Group * group){
    // TODO

    // Step one, we write the meta if it hasn't already been written.
    // The helper handles this, if we set the meta:

    if (! _helper.initialized()){
      _helper.image_meta.clear();
      LARCV_CRITICAL() << "_tensor_v size: " << _tensor_v.size() << std::endl;
      for (auto & sparse_tensor : _tensor_v)
        _helper.image_meta.push_back(sparse_tensor.meta());

      _helper.write_image_meta(group);
  
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
    std::vector<larcv::Voxel> _voxels;


    // User the helper to write them to file:
    _helper.write_voxels(group, _voxels);
    // Serialization relies on the helper to write everything.
    return;

  }

  void EventSparseTensor::deserialize(H5::Group * group, size_t entry){
    // TODO

    // Make sure to read in meta before writing:
    if (! _helper.initialized()){
      _helper.image_meta.clear();
      _helper.read_image_meta(group); 
    }

    return;

  }

  // IO functions: 
  void EventSparseClusters::initialize (H5::Group * group){

    // Call the helper initialize function:
    _helper.initialize_voxel_group(group);


  }

  void EventSparseClusters::serialize  (H5::Group * group){
    // TODO
    return;

  }

  void EventSparseClusters::deserialize(H5::Group * group, size_t entry){
    // TODO

    return;

  }


}

#endif
