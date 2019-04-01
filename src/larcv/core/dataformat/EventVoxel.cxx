#ifndef __LARCV_EVENTVOXEL2D_CXX
#define __LARCV_EVENTVOXEL2D_CXX

#include "EventSparse.h"

#define VOXEL_EXTENTS_CHUNK_SIZE 1
#define VOXEL_DATA_CHUNK_SIZE 64

namespace larcv {

  // /// Global larcv::EventClusterPixel2DFactory to register EventClusterPixel2D
  // static EventClusterPixel2DFactory __global_EventClusterPixel2DFactory__;

  /// Global larcv::EventSparseTensorFactory to register EventSparseTensor
  static EventSparseTensorFactory __global_EventSparseTensorFactory__;

  //
  // EventClusterPixel2D
  // //
  // const larcv::ClusterPixel2D&
  // EventClusterPixel2D::cluster_pixel_2d(const ProjectionID_t id) const
  // {
  //   if(id >= _cluster_v.size()) {
  //     std::cerr << "EventClusterPixel2D does not hold any ClusterPixel2D for ProjectionID_t " << id << std::endl;
  //     throw larbys();
  //   }
  //   return _cluster_v[id];
  // }

  // void EventClusterPixel2D::emplace(larcv::ClusterPixel2D&& clusters)
  // {
  //   if(_cluster_v.size() <= clusters.meta().id())
  //     _cluster_v.resize(clusters.meta().id()+1);
  //   _cluster_v[clusters.meta().id()] = std::move(clusters);
  // }

  // void EventClusterPixel2D::set(const larcv::ClusterPixel2D& clusters) 
  // {
  //   if(_cluster_v.size() <= clusters.meta().id())
  //     _cluster_v.resize(clusters.meta().id()+1);
  //   _cluster_v[clusters.meta().id()] = clusters;
    
  // }
  
  // void EventClusterPixel2D::emplace(larcv::VoxelSetArray&& clusters, larcv::ImageMeta&& meta)
  // {
  //   larcv::ClusterPixel2D source(std::move(clusters),std::move(meta));
  //   emplace(std::move(source));
  // }

  // void EventClusterPixel2D::set(const larcv::VoxelSetArray& clusters, const larcv::ImageMeta& meta)
  // {
  //   larcv::ClusterPixel2D source(clusters);
  //   source.meta(meta);
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
    // This is a tricky situation.
    // Can't call initialize until we know more about the meta information.
    // Can't know more about the meta information until we've called initialize.

    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty SparseTensor group " << group->fromClass() << std::endl;
      throw larbys();
    }

  }

  void EventSparseTensor::serialize  (H5::Group * group){

    // The first step in this function is to determine if this group has been intialized
    // Check if the attribute is available:
    if ( group -> attrExists("initialized")){
        // Do the serialization
    }
    else{
        _initialize(group);
    }

  }

  void EventSparseTensor::deserialize(H5::Group * group, size_t entry){

    return;

  }

  void EventSparseTensor::_initialize( H5::Group * group){

    // This is the REAL initialize function.  It gets called from serialize only.
    // This function will set up the tables correctly the first time it's called, and 
    // will check the dimesionality of the data matches every subsequent time.





    // For voxels, we store a flat table of voxels and an extents table for each projection ID.

    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // particles in the data tree.


    // Get the data type for extents:
    H5::DataType extents_datatype = larcv::get_datatype<Extents_t>();

    // How many projections are we talking about here?
    size_t n_projections = _tensor_v.size();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t extents_starting_dim[] = {n_projections, 0};
    hsize_t extents_maxsize_dim[]  = {n_projections, H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace extents_dataspace(2, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList extents_cparms;
    hsize_t      extents_chunk_dims[2] ={n_projections, VOXEL_EXTENTS_CHUNK_SIZE};
    extents_cparms.setChunk( 2, extents_chunk_dims );

    // Create the extents dataset:
    H5::DataSet extents_ds = group->createDataSet("extents", extents_datatype, extents_dataspace, extents_cparms);


    /////////////////////////////////////////////////////////
    // Create the voxels datasets
    /////////////////////////////////////////////////////////

    // loop over projection IDs
    for (size_t pid = 0; pid < n_projections; ++ pid){

        // The particles table is a flat table of particles, one event appended to another


        // Get the data type for extents:
        H5::DataType voxel_datatype = larcv::Voxel::get_datatype();


        // Get the starting size (0) and dimensions (unlimited)
        hsize_t voxel_starting_dim[] = {0};
        hsize_t voxel_maxsize_dim[]  = {H5S_UNLIMITED};

        // Create a dataspace 
        H5::DataSpace voxel_dataspace(1, voxel_starting_dim, voxel_maxsize_dim);

        /*
         * Modify dataset creation properties, i.e. enable chunking.
         */
        H5::DSetCreatPropList voxel_cparms;
        hsize_t      voxel_chunk_dims[1] ={VOXEL_DATA_CHUNK_SIZE};
        voxel_cparms.setChunk( 1, voxel_chunk_dims );

        // Give this table a unique name:
        std::string name = "voxels" + std::to_string(pid);

        // Create the extents dataset:
        H5::DataSet voxel_ds = group->createDataSet(name, voxel_datatype, voxel_dataspace, voxel_cparms);

    }

    // Lastly, add an attribute to the group to mark it initialized.
    // Once intialized, the data storage model is static.
    H5::DataType attribute_type = larcv::get_datatype<bool>();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t attribute_starting_dim[] = {1};
    hsize_t attribute_maxsize_dim[]  = {1};

    // Create a dataspace 
    H5::DataSpace attribute_dataspace(1, attribute_starting_dim, attribute_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList attribute_cparms;
    // hsize_t      attribute_chunk_dims[2] ={n_projections, VOXEL_attribute_CHUNK_SIZE};
    // attribute_cparms.setChunk( 2, attribute_chunk_dims );

    H5::Attribute init_attrib = group -> createAttribute ("initialized", attribute_type, attribute_dataspace);
    bool att_value = true;

    init_attrib.write(attribute_type, &att_value);
  }


  
}

#endif
