#ifndef __LARCV3DATAFORMAT_EVENTSPARSECLUSTER_CXX
#define __LARCV3DATAFORMAT_EVENTSPARSECLUSTER_CXX

#include "larcv3/core/dataformat/EventSparseCluster.h"

#define VOXEL_EXTENTS_CHUNK_SIZE 10
#define VOXEL_IDEXTENTS_CHUNK_SIZE 100
#define VOXEL_META_CHUNK_SIZE 100
#define VOXEL_DATA_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 100

#define EXTENTS_DATASET 0
#define CLUSTER_EXTENTS_DATASET 1
#define PROJECTION_DATASET 2
#define IMAGE_META_DATASET 3
#define VOXELS_DATASET 4
#define N_DATASETS 5


namespace larcv3 {

  /// Global larcv3::EventClusterPixel2DFactory to register EventSparseCluster
  static EventSparseCluster2DFactory __global_EventSparseCluster2DFactory__;
  static EventSparseCluster3DFactory __global_EventSparseCluster3DFactory__;

  template<size_t dimension>
  EventSparseCluster<dimension>::EventSparseCluster(){

    _data_types.resize(N_DATASETS);

    _data_types[EXTENTS_DATASET]         = larcv3::get_datatype<Extents_t>();
    _data_types[CLUSTER_EXTENTS_DATASET] = larcv3::get_datatype<IDExtents_t>();
    _data_types[PROJECTION_DATASET]      = larcv3::get_datatype<IDExtents_t>();
    _data_types[IMAGE_META_DATASET]      = larcv3::ImageMeta<dimension>::get_datatype();
    _data_types[VOXELS_DATASET]          = larcv3::Voxel::get_datatype();


  }

  // EventSparseCluster
  //
  template<size_t dimension> 
  const larcv3::SparseCluster<dimension>&
  EventSparseCluster<dimension>::sparse_cluster(const ProjectionID_t id) const
  {
    if(id >= _cluster_v.size()) {
      std::cerr << "EventSparseCluster does not hold any SparseCluster for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _cluster_v[id];
  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::emplace(larcv3::SparseCluster<dimension>&& clusters)
  {
    if(_cluster_v.size() <= clusters.meta().id())
      _cluster_v.resize(clusters.meta().id()+1);
    _cluster_v[clusters.meta().id()] = std::move(clusters);
  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::set(const larcv3::SparseCluster<dimension>& clusters) 
  {
    if(_cluster_v.size() <= clusters.meta().id())
      _cluster_v.resize(clusters.meta().id()+1);
    _cluster_v[clusters.meta().id()] = clusters;
    
  }
  
  template<size_t dimension> 
  void EventSparseCluster<dimension>::emplace(larcv3::VoxelSetArray&& clusters, larcv3::ImageMeta<dimension>&& meta)
  {
    larcv3::SparseCluster<dimension> source(std::move(clusters),std::move(meta));
    // source.set();
    emplace(std::move(source));
  }

  // template<size_t dimension> 
  // void EventSparseCluster<dimension>::set(const larcv3::VoxelSetArray& clusters, const larcv3::ImageMeta<dimension>& meta)
  // {

  //   if(_cluster_v.size() <= clusters.meta().id())
  //     _cluster_v.resize(clusters.meta().id()+1);
  //   _cluster_v[clusters.meta().id()] = clusters;
    

  //   larcv3::SparseCluster source;
  //   source.emplace(std::move(clusters), meta);
  //   // source.meta(meta);
  //   emplace(std::move(source));
  // }

  // IO functions: 
  template<size_t dimension> 
  void EventSparseCluster<dimension>::initialize (hid_t group, uint compression){

    if (get_num_objects(group) != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group << std::endl;
      throw larbys();
    }


    // Initialization creates 5 tables for a set of voxels:
    // Extents (Traditional extents, but maps to the next table)
    // ProjectionExtents (Extents but with an ID for each projection ID)
    // VoxelMeta (ImageMeta that is specific to a projection)
    // ClusterExtents (IDExtents for each cluster within a projection)
    // Voxels (A big table of voxels.)


    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // IDExtents in the data tree.



    // Get the starting size (0) and dimensions (unlimited)
    hsize_t extents_starting_dim[] = {0};
    hsize_t extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t extents_dataspace = H5Screate_simple(1, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */

    hid_t extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList extents_cparms;
    hsize_t      extents_chunk_dims[1] ={VOXEL_EXTENTS_CHUNK_SIZE};
    H5Pset_chunk(extents_cparms, 1, extents_chunk_dims );
    if (compression){
      H5Pset_deflate(extents_cparms, compression);
      // extents_cparms.setDeflate(compression);
    }

    // Create the extents dataset:
    H5Dcreate(
      group,                        // hid_t loc_id  IN: Location identifier
      "extents",                    // const char *name      IN: Dataset name
      _data_types[EXTENTS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      extents_dataspace,            // hid_t space_id  IN: Dataspace identifier
      NULL,                         // hid_t lcpl_id IN: Link creation property list
      extents_cparms,               // hid_t dcpl_id IN: Dataset creation property list
      NULL                          // hid_t dapl_id IN: Dataset access property list
    );

    /////////////////////////////////////////////////////////
    // Create the ID extents dataset (ProjectionExtents)
    /////////////////////////////////////////////////////////


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t projection_extents_starting_dim[] = {0};
    hsize_t projection_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t projection_extents_dataspace = H5Screate_simple(1, projection_extents_starting_dim, projection_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */

    hid_t projection_extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList extents_cparms;
    hsize_t      projection_extents_chunk_dims[1] ={VOXEL_IDEXTENTS_CHUNK_SIZE};
    H5Pset_chunk(projection_extents_cparms, 1, projection_extents_chunk_dims );
    if (compression){
      H5Pset_deflate(projection_extents_cparms, compression);
      // extents_cparms.setDeflate(compression);
    }


    // Create the ID extents dataset:
    H5Dcreate(
      group,                           // hid_t loc_id  IN: Location identifier
      "projection_extents",            // const char *name      IN: Dataset name
      _data_types[PROJECTION_DATASET], // hid_t dtype_id  IN: Datatype identifier
      projection_extents_dataspace,    // hid_t space_id  IN: Dataspace identifier
      NULL,                            // hid_t lcpl_id IN: Link creation property list
      projection_extents_cparms,       // hid_t dcpl_id IN: Dataset creation property list
      NULL                             // hid_t dapl_id IN: Dataset access property list
    );



    /////////////////////////////////////////////////////////
    // Create the meta dataset (ImageMeta<dimension>)
    /////////////////////////////////////////////////////////
    
    // Get the starting size (0) and dimensions (unlimited)
    hsize_t image_meta_starting_dim[] = {0};
    hsize_t image_meta_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t image_meta_dataspace = H5Screate_simple(1, image_meta_starting_dim, image_meta_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */

    hid_t image_meta_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList extents_cparms;
    hsize_t      image_meta_chunk_dims[1] ={IMAGE_META_CHUNK_SIZE};
    H5Pset_chunk(image_meta_cparms, 1, image_meta_chunk_dims );
    if (compression){
      H5Pset_deflate(image_meta_cparms, compression);
      // extents_cparms.setDeflate(compression);
    }


    // Create the extents dataset:
    H5Dcreate(
      group,                           // hid_t loc_id  IN: Location identifier
      "image_meta",                    // const char *name      IN: Dataset name
      _data_types[IMAGE_META_DATASET], // hid_t dtype_id  IN: Datatype identifier
      image_meta_dataspace,            // hid_t space_id  IN: Dataspace identifier
      NULL,                            // hid_t lcpl_id IN: Link creation property list
      image_meta_cparms,               // hid_t dcpl_id IN: Dataset creation property list
      NULL                             // hid_t dapl_id IN: Dataset access property list
    );

    /////////////////////////////////////////////////////////
    // Create the Cluster extents dataset (IDExtents)
    /////////////////////////////////////////////////////////


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t cluster_extents_starting_dim[] = {0};
    hsize_t cluster_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t cluster_extents_dataspace = H5Screate_simple(1, cluster_extents_starting_dim, cluster_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */

    hid_t cluster_extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList extents_cparms;
    hsize_t      cluster_extents_chunk_dims[1] ={VOXEL_META_CHUNK_SIZE};
    H5Pset_chunk(cluster_extents_cparms, 1, cluster_extents_chunk_dims );
    if (compression){
      H5Pset_deflate(cluster_extents_cparms, compression);
      // extents_cparms.setDeflate(compression);
    }

    // Create the extents dataset:
    H5Dcreate(
      group,                                // hid_t loc_id  IN: Location identifier
      "cluster_extents",                    // const char *name      IN: Dataset name
      _data_types[CLUSTER_EXTENTS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      cluster_extents_dataspace,            // hid_t space_id  IN: Dataspace identifier
      NULL,                                 // hid_t lcpl_id IN: Link creation property list
      cluster_extents_cparms,               // hid_t dcpl_id IN: Dataset creation property list
      NULL                                  // hid_t dapl_id IN: Dataset access property list
    );


    /////////////////////////////////////////////////////////
    // Create the voxels dataset (Voxels)
    /////////////////////////////////////////////////////////


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t voxel_starting_dim[] = {0};
    hsize_t voxel_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t voxel_dataspace = H5Screate_simple(1, voxel_starting_dim, voxel_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */

    hid_t voxel_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList extents_cparms;
    hsize_t      voxel_chunk_dims[1] ={VOXEL_DATA_CHUNK_SIZE};
    H5Pset_chunk(voxel_cparms, 1, voxel_chunk_dims );
    if (compression){
      H5Pset_deflate(voxel_cparms, compression);
      // extents_cparms.setDeflate(compression);
    }

    // Create the voxels dataset:
    H5Dcreate(
      group,                       // hid_t loc_id  IN: Location identifier
      "voxels",                    // const char *name      IN: Dataset name
      _data_types[VOXELS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      voxel_dataspace,             // hid_t space_id  IN: Dataspace identifier
      NULL,                        // hid_t lcpl_id IN: Link creation property list
      voxel_cparms,                // hid_t dcpl_id IN: Dataset creation property list
      NULL                         // hid_t dapl_id IN: Dataset access property list
    );


  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::open_in_datasets(hid_t group){

    if (_open_in_datasets.size() < N_DATASETS ){
       _open_in_datasets.resize(N_DATASETS);
       _open_in_dataspaces.resize(N_DATASETS);
       

       _open_in_datasets[EXTENTS_DATASET]            = H5Dopen(group, "extents", H5P_DEFAULT);
       _open_in_dataspaces[EXTENTS_DATASET]          = H5Dget_space(_open_in_datasets[EXTENTS_DATASET]);

       _open_in_datasets[PROJECTION_DATASET]         = H5Dopen(group, "projection_extents", H5P_DEFAULT);
       _open_in_dataspaces[PROJECTION_DATASET]       = H5Dget_space(_open_in_datasets[PROJECTION_DATASET]);

       _open_in_datasets[CLUSTER_EXTENTS_DATASET]    = H5Dopen(group, "cluster_extents", H5P_DEFAULT);
       _open_in_dataspaces[CLUSTER_EXTENTS_DATASET]  = H5Dget_space(_open_in_datasets[CLUSTER_EXTENTS_DATASET]);

       _open_in_datasets[IMAGE_META_DATASET]         = H5Dopen(group, "image_meta", H5P_DEFAULT);
       _open_in_dataspaces[IMAGE_META_DATASET]       = H5Dget_space(_open_in_datasets[IMAGE_META_DATASET]);

       _open_in_datasets[VOXELS_DATASET]             = H5Dopen(group, "voxels", H5P_DEFAULT);
       _open_in_dataspaces[VOXELS_DATASET]           = H5Dget_space(_open_in_datasets[VOXELS_DATASET]);
     }

    return;
  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::open_out_datasets(hid_t group){

    if (_open_out_datasets.size() < N_DATASETS ){
       _open_out_datasets.resize(N_DATASETS);
       _open_out_dataspaces.resize(N_DATASETS);
       
       _open_out_datasets[EXTENTS_DATASET]            = H5Dopen(group, "extents", H5P_DEFAULT);
       _open_out_dataspaces[EXTENTS_DATASET]          = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);

       _open_out_datasets[PROJECTION_DATASET]         = H5Dopen(group, "projection_extents", H5P_DEFAULT);
       _open_out_dataspaces[PROJECTION_DATASET]       = H5Dget_space(_open_out_datasets[PROJECTION_DATASET]);

       _open_out_datasets[CLUSTER_EXTENTS_DATASET]    = H5Dopen(group, "cluster_extents", H5P_DEFAULT);
       _open_out_dataspaces[CLUSTER_EXTENTS_DATASET]  = H5Dget_space(_open_out_datasets[CLUSTER_EXTENTS_DATASET]);

       _open_out_datasets[IMAGE_META_DATASET]         = H5Dopen(group, "image_meta", H5P_DEFAULT);
       _open_out_dataspaces[IMAGE_META_DATASET]       = H5Dget_space(_open_out_datasets[IMAGE_META_DATASET]);

       _open_out_datasets[VOXELS_DATASET]             = H5Dopen(group, "voxels", H5P_DEFAULT);
       _open_out_dataspaces[VOXELS_DATASET]           = H5Dget_space(_open_out_datasets[VOXELS_DATASET]);
    }

    return;
  }

  template<size_t dimension>
  void EventSparseCluster<dimension>::finalize(){
    for (size_t i = 0; i < _open_in_datasets.size(); i ++){
      H5Sclose(_open_in_dataspaces[i]);
      H5Dclose(_open_in_datasets[i]);
    } 
    for (size_t i = 0; i < _open_out_datasets.size(); i ++){
      H5Sclose(_open_in_dataspaces[i]);
      H5Dclose(_open_out_datasets[i]);
    } 
  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::serialize  (hid_t group){
  

    // Sparse cluster write one meta per projection ID.  So, the formatting of
    // the datasets is: 
    // Overall extents table indicates the right entries (projection IDs) in the IDExtents table
    // IDExtents table contains a list of projection IDs in order, as well as the
    // corresponding cluster ranges in the Cluster table.
    // IDExtents is also one-to-one (and therefore mapped to by the extents table)
    // to a meta table.
    // ClusterIDs contains the cluster Extents object, which maps to the voxel table

    // Here is the order of operations for voxel serialization:
    // 1) Read the current dimensions of all tables (extents, projection_extents, image_meta, cluster_extents, voxels)
    // 2) Using the dimensions of the projection table, build the projection_extents object for this event
    // 2a) Using the same dimensions as 2), build the image_meta object for this event
    // 3) Using the dimensions of the projection_events table, and the dimensions of this event's projection_events, 
    //    update the extents table
    // 4) Update the projection_extents table with the projection info for this event
    // 5) Update the image_meta table with the meta vector for this object.
    // 6) Update the cluster_extents table with the cluster_extents vector for this object.
    // 7) Update the voxels table with the voxels from this event, using the cluster_extents vector
    open_out_datasets(group);

    // Transfer property list, default:
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);


    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////


    // Get the dataset current size
    hsize_t extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[EXTENTS_DATASET], extents_dims_current, NULL);


    // Get the dataset current size
    hsize_t projection_extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[PROJECTION_DATASET], projection_extents_dims_current, NULL);


    // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[IMAGE_META_DATASET], image_meta_dims_current, NULL);

    // Get the dataset current size
    hsize_t cluster_extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[CLUSTER_EXTENTS_DATASET], cluster_extents_dims_current, NULL);


    // Get the dataset current size
    hsize_t voxels_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[VOXELS_DATASET], voxels_dims_current, NULL);


    /////////////////////////////////////////////////////////
    // Step 2: Build the projection_extents object
    /////////////////////////////////////////////////////////

    // We need to make the voxel extents object first, which we can do from the vector of voxels.
    std::vector<IDExtents_t> projection_extents;

    size_t last_cluster_index = cluster_extents_dims_current[0];
    size_t n_new_projections = 0;


    for (size_t projection_id = 0; projection_id < _cluster_v.size(); projection_id ++){
      projection_extents.resize(projection_extents.size() + 1);
      projection_extents.back().n     = _cluster_v.at(projection_id).size();
      projection_extents.back().id    = _cluster_v.at(projection_id).meta().projection_id();
      projection_extents.back().first = last_cluster_index;
      last_cluster_index += projection_extents.back().n;
      n_new_projections += projection_extents.back().n;
    }

    // std::cout << "Voxel Extents Size: " << projection_extents.size() << std::endl;
    // for (auto & e : projection_extents){
    //   std::cout << "  n: " << e.n << std::endl;
    // }


    /////////////////////////////////////////////////////////
    // Step 2a: Build the image_meta object
    /////////////////////////////////////////////////////////

    std::vector<ImageMeta<dimension> > image_meta;

    for (size_t projection_id = 0; projection_id < _cluster_v.size(); projection_id ++){
      image_meta.push_back(_cluster_v.at(projection_id).meta());
    }

    /////////////////////////////////////////////////////////
    // Step 3: Update the overall extents table
    /////////////////////////////////////////////////////////

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t extents_size[1];
    extents_size[0] = extents_dims_current[0] + extents_slab_dims[0];


    // Extend the dataset to accomodate the new data
    // _open_out_datasets[EXTENTS_DATASET].extend(extents_size);
    H5Dset_extent(_open_out_datasets[EXTENTS_DATASET], extents_size);

    // Create an extents object to go into the extents table:

    Extents_t next_extents;
    next_extents.first = projection_extents_dims_current[0];
    next_extents.n = projection_extents.size();


    /////////////////////////////////////////////////////////
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_dims_current, // start
      NULL ,                  // stride
      extents_slab_dims,    // count 
      NULL                    // block
      );

    // Define memory space:
    // H5::DataSpace particles_memspace(1, particles_slab_dims);
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);


    H5Dwrite(_open_out_datasets[EXTENTS_DATASET],   // dataset_id,
             _data_types[EXTENTS_DATASET],          // hit_t mem_type_id, 
             extents_memspace,                      // hid_t mem_space_id, 
             _open_out_dataspaces[EXTENTS_DATASET], // hid_t file_space_id, 
             xfer_plist_id,                         // hid_t xfer_plist_id, 
             &(next_extents)                        // const void * buf 
           );



    /////////////////////////////////////////////////////////
    // Step 4: Update the projection extents table
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t projection_extents_slab_dims[1];
    projection_extents_slab_dims[0] = projection_extents.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t projection_extents_size[1];
    projection_extents_size[0] = projection_extents_dims_current[0] + projection_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    // _open_out_datasets[PROJECTION_DATASET].extend(projection_extents_size);
    H5Dset_extent(_open_out_datasets[PROJECTION_DATASET], projection_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new projection_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[PROJECTION_DATASET] = H5Dget_space(_open_out_datasets[PROJECTION_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[PROJECTION_DATASET], 
      H5S_SELECT_SET, 
      projection_extents_dims_current, // start
      NULL ,                           // stride
      projection_extents_slab_dims,    // count 
      NULL                             // block
    );



    // Define memory space:
    hid_t projection_extents_memspace = H5Screate_simple(1, projection_extents_slab_dims, NULL);


    // Write the new data
    H5Dwrite(_open_out_datasets[PROJECTION_DATASET],   // dataset_id,
             _data_types[PROJECTION_DATASET],          // hit_t mem_type_id, 
             projection_extents_memspace,              // hid_t mem_space_id, 
             _open_out_dataspaces[PROJECTION_DATASET], // hid_t file_space_id, 
             xfer_plist_id,                            // hid_t xfer_plist_id, 
             &(projection_extents[0])                  // const void * buf 
           );

    /////////////////////////////////////////////////////////
    // Step 5: Write image meta
    /////////////////////////////////////////////////////////

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = image_meta.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t image_meta_size[1];
    image_meta_size[0] = image_meta_dims_current[0] + image_meta_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[IMAGE_META_DATASET], image_meta_size);


    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[IMAGE_META_DATASET] = H5Dget_space(_open_out_datasets[IMAGE_META_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[IMAGE_META_DATASET], 
      H5S_SELECT_SET, 
      image_meta_dims_current, // start
      NULL ,                   // stride
      image_meta_slab_dims,    // count 
      NULL                     // block
    );

    // Define memory space:
    hid_t image_meta_memspace = H5Screate_simple(1, image_meta_slab_dims, NULL);


    // Write the new data
    H5Dwrite(_open_out_datasets[IMAGE_META_DATASET],   // dataset_id,
             _data_types[IMAGE_META_DATASET],          // hit_t mem_type_id, 
             image_meta_memspace,                      // hid_t mem_space_id, 
             _open_out_dataspaces[IMAGE_META_DATASET], // hid_t file_space_id, 
             xfer_plist_id,                            // hid_t xfer_plist_id, 
             &(image_meta[0])                          // const void * buf 
           );
    /////////////////////////////////////////////////////////
    // Step 4: Update the cluster extents table
    /////////////////////////////////////////////////////////

    // First, build the cluster extents object, which has an entry for every cluster
    // It is flattened, with the ragged indexing specified by the projection_extents table

    // The 'first' and 'n' objects refer to the voxel table, so we need to know the current values:


    std::vector<IDExtents_t> cluster_extents;
    size_t last_voxel_index = voxels_dims_current[0];
    size_t n_new_voxels = 0;

    for (size_t projection_id = 0; projection_id < _cluster_v.size(); projection_id ++){
      for (size_t cluster_id = 0; cluster_id < _cluster_v.at(projection_id).size(); cluster_id ++){
        cluster_extents.resize(cluster_extents.size() + 1);
        cluster_extents.back().n     = _cluster_v.at(projection_id).as_vector().at(cluster_id).size();
        cluster_extents.back().id    = _cluster_v.at(projection_id).as_vector().at(cluster_id).id();
        cluster_extents.back().first = last_voxel_index;
        last_voxel_index += cluster_extents.back().n;
        n_new_voxels += cluster_extents.back().n;
      }
      
    }


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t cluster_extents_slab_dims[1];
    cluster_extents_slab_dims[0] = cluster_extents.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t cluster_extents_size[1];
    cluster_extents_size[0] = cluster_extents_dims_current[0] + cluster_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[CLUSTER_EXTENTS_DATASET], cluster_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new cluster_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[CLUSTER_EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[CLUSTER_EXTENTS_DATASET]);
    H5Sselect_hyperslab(
      _open_out_dataspaces[CLUSTER_EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      cluster_extents_dims_current, // start
      NULL ,                        // stride
      cluster_extents_slab_dims,    // count 
      NULL                          // block
    );
    // Define memory space:
    hid_t cluster_extents_memspace = H5Screate_simple(1, cluster_extents_slab_dims, NULL);


    // Write the new data

    H5Dwrite(
      _open_out_datasets[CLUSTER_EXTENTS_DATASET],   // dataset_id,
      _data_types[CLUSTER_EXTENTS_DATASET],          // hit_t mem_type_id, 
      cluster_extents_memspace,                      // hid_t mem_space_id, 
      _open_out_dataspaces[CLUSTER_EXTENTS_DATASET], // hid_t file_space_id, 
      xfer_plist_id,                                 // hid_t xfer_plist_id, 
      &(cluster_extents[0])                          // const void * buf 
    );
    /////////////////////////////////////////////////////////
    // Step 6: Write new voxels
    /////////////////////////////////////////////////////////

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t voxels_slab_dims[1];
    voxels_slab_dims[0] = n_new_voxels;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t voxels_size[1];
    voxels_size[0] = voxels_dims_current[0] + voxels_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[VOXELS_DATASET], voxels_size);


    _open_out_dataspaces[VOXELS_DATASET] = H5Dget_space(_open_out_datasets[VOXELS_DATASET]);
    // Write all of the voxels to file:

    // Keep track of starting index for a write:
    size_t starting_index = voxels_dims_current[0];

    for (size_t projection_id = 0; projection_id < _cluster_v.size(); projection_id ++){
      for (size_t cluster_id = 0; cluster_id < _cluster_v.at(projection_id).size(); cluster_id ++){
        hsize_t new_voxels_slab_dims[1];
        hsize_t offset_voxels_slab_dims[1];

        new_voxels_slab_dims[0] = _cluster_v.at(projection_id).as_vector().at(cluster_id).size();
        offset_voxels_slab_dims[0] = starting_index;

        // std::cout << "[" << projection_id << "][" << cluster_id << "]: \n"
        //           << "  Offset: " << offset_voxels_slab_dims[0]
        //           << "\n  starting_index: " << starting_index 
        //           << "\n  N: " << new_voxels_slab_dims[0]
        //           << "\n  N: " << _cluster_v.at(projection_id).at(cluster_id).size()
        //           << std::endl;

        // Select as a hyperslab the last section of data for writing:
        H5Sselect_hyperslab(
          _open_out_dataspaces[VOXELS_DATASET], 
          H5S_SELECT_SET, 
          offset_voxels_slab_dims, // start
          NULL ,                   // stride
          new_voxels_slab_dims,    // count 
          NULL                     // block
        );
        // Define memory space:
        hid_t voxels_memspace = H5Screate_simple(1, new_voxels_slab_dims, NULL);

        // Write the new data
        H5Dwrite(
          _open_out_datasets[VOXELS_DATASET],   // dataset_id,
          _data_types[VOXELS_DATASET],          // hit_t mem_type_id, 
          voxels_memspace,                      // hid_t mem_space_id, 
          _open_out_dataspaces[VOXELS_DATASET], // hid_t file_space_id, 
          xfer_plist_id,                        // hid_t xfer_plist_id, 
          &(_cluster_v.at(projection_id).as_vector().at(cluster_id).as_vector()[0]) // const void * buf 
        );
        starting_index += new_voxels_slab_dims[0];
      }
    }




    return;

  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::deserialize(hid_t group, size_t entry, bool reopen_groups){


    // This function reads in a set of voxels for sparse clusters
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the projection_extents table information
    // 3) Use the entry information to get the image_meta table information
    // 4) Use the projection_extents information to get the cluster extents information
    // 5) Use the cluster_extents information to read the correct voxels
    // 6) Update the meta for each set correctly
    
    if (reopen_groups){
      _open_in_dataspaces.clear();
      _open_in_datasets.clear();
    }

    open_in_datasets(group);
    /////////////////////////////////////////////////////////
    // Step 1: Get the extents information from extents dataset
    /////////////////////////////////////////////////////////


    // Get a dataspace inside this file:
    // H5::DataSpace _open_datapaces[EXTENTS_DATASET] = extents_dataset.getSpace();


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;

    hsize_t extents_offset[1];
    extents_offset[0] = entry;

    // Transfer property list, default
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);

    /////////////////////////////////////////////////////////
    // Read this extents entry from the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    // _open_datapaces[EXTENTS_DATASET] = extents_dataset.getSpace();

    H5Sselect_hyperslab(_open_in_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_offset, // start
      NULL ,  // stride
      extents_slab_dims, //count 
      NULL // block
      );


    Extents_t input_extents;

    // Define memory space:
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);

    // Write the new data
    // _open_in_datasets[EXTENTS_DATASET].read(&(input_extents), *_data_types[EXTENTS_DATASET],
      // extents_memspace, _open_in_dataspaces[EXTENTS_DATASET]);
    

    H5Dread(
      _open_in_datasets[EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                         // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(input_extents)                       // void * buf  OUT: Buffer to receive data read from file.
    );

    /////////////////////////////////////////////////////////
    // Step 2: Get the projection_extents information
    /////////////////////////////////////////////////////////


    // Next, open the relevant sections of the data 

    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t projection_extents_slab_dims[1];
    projection_extents_slab_dims[0] = input_extents.n;

    hsize_t projection_extents_offset[1];
    projection_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:

    H5Sselect_hyperslab(_open_in_dataspaces[PROJECTION_DATASET], 
      H5S_SELECT_SET, 
      projection_extents_offset,    // start
      NULL ,                        // stride
      projection_extents_slab_dims, //count 
      NULL                          // block
    );

    hid_t projection_extents_memspace = H5Screate_simple(1, projection_extents_slab_dims, NULL);


    std::vector<IDExtents_t> projection_extents;

    // Reserve space for reading in projection_extents:
    projection_extents.resize(input_extents.n);

    H5Dread(
      _open_in_datasets[PROJECTION_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[PROJECTION_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      projection_extents_memspace,              // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[PROJECTION_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                            // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(projection_extents[0])                  // void * buf  OUT: Buffer to receive data read from file.
    );
    // std::cout << "voxel_extents.size(): " << voxel_extents.size() << std::endl;



    /////////////////////////////////////////////////////////
    // Step 3: Get the image_meta information
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = input_extents.n;

    hsize_t image_meta_offset[1];
    image_meta_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for reading:
    H5Sselect_hyperslab(_open_in_dataspaces[IMAGE_META_DATASET], 
      H5S_SELECT_SET, 
      image_meta_offset,    // start
      NULL ,                // stride
      image_meta_slab_dims, //count 
      NULL                  // block
    );

    hid_t image_meta_memspace = H5Screate_simple(1, image_meta_slab_dims, NULL);

    std::vector<ImageMeta<dimension> > image_meta;

    // Reserve space for reading in image_meta:
    image_meta.resize(input_extents.n);

    H5Dread(
      _open_in_datasets[IMAGE_META_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[IMAGE_META_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      image_meta_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[IMAGE_META_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                            // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(image_meta[0])                          // void * buf  OUT: Buffer to receive data read from file.
    );
    // std::cout << "image_meta.size(): " << image_meta.size() << std::endl;
        
    /////////////////////////////////////////////////////////
    // Step 4: Read the cluster_extents
    /////////////////////////////////////////////////////////


    // Loop over the projection extents to find out the cluster_extents:
    size_t n_total_clusters = 0;
    for (size_t projection_id = 0; projection_id < projection_extents.size(); projection_id ++)
      n_total_clusters += projection_extents.at(projection_id).n;

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t cluster_extents_slab_dims[1];
    cluster_extents_slab_dims[0] = n_total_clusters;

    hsize_t cluster_extents_offset[1];
    cluster_extents_offset[0] = projection_extents.front().first;

    // Now, select as a hyperslab the last section of data for reading:
    H5Sselect_hyperslab(_open_in_dataspaces[CLUSTER_EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      cluster_extents_offset,    // start
      NULL ,                     // stride
      cluster_extents_slab_dims, //count 
      NULL                       // block
    );

    hid_t cluster_extents_memspace = H5Screate_simple(1, cluster_extents_slab_dims, NULL);

    std::vector<IDExtents_t> cluster_extents;

    // Reserve space for reading in cluster_extents:
    cluster_extents.resize(n_total_clusters);

    H5Dread(
      _open_in_datasets[CLUSTER_EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[CLUSTER_EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      cluster_extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[CLUSTER_EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                                 // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(cluster_extents[0])                          // void * buf  OUT: Buffer to receive data read from file.
    );
    // std::cout << "voxel_extents.size(): " << voxel_extents.size() << std::endl;


    /////////////////////////////////////////////////////////
    // Step 5: Read the voxels
    /////////////////////////////////////////////////////////

    // At this point, we know the following:
    // - How many projections there are (image_meta.size())
    // - How many total clusters there are (cluster_extents.size())
    //
    // To make things useful, we untangle the the clusters per projection

    _cluster_v.clear();
    _cluster_v.resize(image_meta.size());

    size_t offset = cluster_extents.front().first;

    size_t i_flat_cluster_index = 0;
    for (size_t projection_id = 0; projection_id < projection_extents.size(); projection_id ++){
      // Set the meta for this projection id:
      _cluster_v.at(projection_id).meta(image_meta.at(projection_id));

      // Make space for clusters:
      _cluster_v.at(projection_id).resize(projection_extents.at(projection_id).n);

      for (size_t cluster_id = 0; cluster_id < projection_extents.at(projection_id).n; cluster_id ++){


        // What cluster is this?
        IDExtents_t this_cluster_extent = cluster_extents.at(i_flat_cluster_index);

        // Create a dimension for the data to add (which is the hyperslab data)
        hsize_t voxels_slab_dims[1];
        voxels_slab_dims[0] = this_cluster_extent.n;

        hsize_t voxels_offset[1];
        voxels_offset[0] = offset;

        // std::cout << "N: " << voxels_slab_dims[0] << "\n"
        //           << "offset: " << voxels_offset[0] << "\n"
        //           << std::endl;
        // Now, select as a hyperslab the last section of data for readomg:
        H5Sselect_hyperslab(_open_in_dataspaces[VOXELS_DATASET], 
          H5S_SELECT_SET, 
          voxels_offset,    // start
          NULL ,            // stride
          voxels_slab_dims, // count 
          NULL              // block
        );

        hid_t voxels_memspace = H5Screate_simple(1, voxels_slab_dims, NULL);

        //// TODO
        // This implementation is not ideal.
        // It copies from disk, then is copying into a vector

        std::vector<larcv3::Voxel> temp_voxel_vector;
        temp_voxel_vector.resize(voxels_slab_dims[0]);

        // Reserve space for reading in voxels:

        H5Dread(
          _open_in_datasets[VOXELS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
          _data_types[VOXELS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
          voxels_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
          _open_in_dataspaces[VOXELS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
          xfer_plist_id,                        // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
          &(temp_voxel_vector[0])               // void * buf  OUT: Buffer to receive data read from file.
        );
        // std::cout << "temp_voxel_vector.size(): " << temp_voxel_vector.size() << std::endl;


        for (auto & v : temp_voxel_vector){
          // if (v.id() >= _cluster_v.at(projection_id).meta().total_voxels()) continue;
          _cluster_v.at(projection_id).writeable_voxel_set(cluster_id).add(v);
        }
        _cluster_v.at(projection_id).writeable_voxel_set(cluster_id).id(this_cluster_extent.id);
        // std::cout << "_cluster_v.at(projection_id).at(cluster_id).size(): " << _cluster_v.at(projection_id).at(cluster_id).size() << std::endl;
        offset += voxels_slab_dims[0];
        i_flat_cluster_index += 1;
      }
    }

    return;

  }


template class EventSparseCluster<2>;
template class EventSparseCluster<3>;
}

#endif
