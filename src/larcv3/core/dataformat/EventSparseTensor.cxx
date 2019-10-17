#ifndef __LARCV3DATAFORMAT_EVENTSPARSETENSOR_CXX
#define __LARCV3DATAFORMAT_EVENTSPARSETENSOR_CXX

#define VOXEL_EXTENTS_CHUNK_SIZE 10
#define VOXEL_IDEXTENTS_CHUNK_SIZE 100
#define VOXEL_META_CHUNK_SIZE 100
#define VOXEL_DATA_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 100

#define EXTENTS_DATASET 0
#define VOXEL_EXTENTS_DATASET 1
#define IMAGE_META_DATASET 2
#define VOXELS_DATASET 3
#define N_DATASETS 4

#include "larcv3/core/dataformat/EventSparseTensor.h"


namespace larcv3 {

  /// Global larcv3::EventSparseTensorFactory to register EventSparseTensor
  static EventSparseTensorFactory<2> __global_EventSparseTensor2DFactory__;
  static EventSparseTensorFactory<3> __global_EventSparseTensor3DFactory__;

  
  template<size_t dimension> 
  EventSparseTensor<dimension>::EventSparseTensor(){

    _data_types.resize(N_DATASETS);

    _data_types[EXTENTS_DATASET]       = larcv3::get_datatype<Extents_t>();
    _data_types[VOXEL_EXTENTS_DATASET] = larcv3::get_datatype<IDExtents_t>();
    _data_types[IMAGE_META_DATASET]    = larcv3::ImageMeta<dimension>::get_datatype();
    _data_types[VOXELS_DATASET]        = larcv3::Voxel::get_datatype();


  }

  //
  // EventSparseTensor
  //
  template<size_t dimension> 
  const larcv3::SparseTensor<dimension>&
  EventSparseTensor<dimension>::sparse_tensor(const ProjectionID_t id) const
  {
    if(id >= _tensor_v.size()) {
      std::cerr << "EventSparseTensor does not hold any SparseTensor for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _tensor_v[id];
  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::emplace(larcv3::SparseTensor<dimension>&& voxels)
  {
    if(_tensor_v.size() <= voxels.meta().id())
      _tensor_v.resize(voxels.meta().id()+1);
    _tensor_v[voxels.meta().id()] = std::move(voxels);

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::set(const larcv3::SparseTensor<dimension>& voxels) 
  {
    if(_tensor_v.size() <= voxels.meta().id())
      _tensor_v.resize(voxels.meta().id()+1);
    _tensor_v[voxels.meta().id()] = voxels;
  }
  
  template<size_t dimension> 
  void EventSparseTensor<dimension>::emplace(larcv3::VoxelSet&& voxels, larcv3::ImageMeta<dimension>&& meta)
  {
    larcv3::SparseTensor<dimension> source(std::move(voxels),std::move(meta));
    emplace(std::move(source));
  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::set(const larcv3::VoxelSet& voxels, const larcv3::ImageMeta<dimension>& meta)
  {
    larcv3::SparseTensor<dimension> source;
    source.set(voxels, meta);
    // source.meta(meta);
    emplace(std::move(source));
  }

  // IO functions: 
  template<size_t dimension> 
  void EventSparseTensor<dimension>::initialize (hid_t group, uint compression){

    if (get_num_objects(group) != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group << std::endl;
      throw larbys();
    }

    // Initialization creates 4 tables for a set of voxels:
    // Extents (Traditional extents, but maps to the next table)
    // VoxelExtents (Extents but with an ID for each entry)
    // VoxelMeta (ImageMeta that is specific to a projection)
    // Voxels (A big table of voxels.)


    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // IDExtents in the data tree.


    // Get the data type for extents:
    // H5::DataType extents_datatype = larcv3::get_datatype<Extents_t>();


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
    // Create the ID extents dataset (VoxelExtents)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    // H5::DataType id_extents_datatype = larcv3::get_datatype<IDExtents_t>();


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t id_extents_starting_dim[] = {0};
    hsize_t id_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t id_extents_dataspace = H5Screate_simple(1, id_extents_starting_dim, id_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t id_extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList id_extents_cparms;
    hsize_t      id_extents_chunk_dims[1] ={VOXEL_IDEXTENTS_CHUNK_SIZE};
    H5Pset_chunk(id_extents_cparms, 1, id_extents_chunk_dims );
    if (compression){
      H5Pset_deflate(id_extents_cparms, compression);
    }


    // Create the extents dataset:
    H5Dcreate(
      group,                              // hid_t loc_id  IN: Location identifier
      "voxel_extents",                    // const char *name      IN: Dataset name
      _data_types[VOXEL_EXTENTS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      id_extents_dataspace,               // hid_t space_id  IN: Dataspace identifier
      NULL,                               // hid_t lcpl_id IN: Link creation property list
      id_extents_cparms,                  // hid_t dcpl_id IN: Dataset creation property list
      NULL                                // hid_t dapl_id IN: Dataset access property list
    );


    /////////////////////////////////////////////////////////
    // Create the meta dataset (ImageMeta<dimension>)
    /////////////////////////////////////////////////////////
    
    // Get the data type for image meta:
    // H5::DataType image_meta_datatype = larcv3::ImageMeta<dimension>::get_datatype();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t image_meta_starting_dim[] = {0};
    hsize_t image_meta_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t image_meta_dataspace = H5Screate_simple(1, image_meta_starting_dim, image_meta_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t image_meta_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList image_meta_cparms;
    hsize_t      image_meta_chunk_dims[1] ={IMAGE_META_CHUNK_SIZE};
    H5Pset_chunk(image_meta_cparms, 1, image_meta_chunk_dims );
    if (compression){
      H5Pset_deflate(image_meta_cparms, compression);
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
    // Create the voxels dataset (Voxels)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    // H5::DataType voxel_datatype = larcv3::Voxel::get_datatype();


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t voxel_starting_dim[] = {0};
    hsize_t voxel_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t voxel_dataspace = H5Screate_simple(1, voxel_starting_dim, voxel_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */


    hid_t voxel_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList voxel_cparms;
    hsize_t      voxel_chunk_dims[1] ={VOXEL_DATA_CHUNK_SIZE};
    H5Pset_chunk(voxel_cparms, 1, voxel_chunk_dims );
    if (compression){
      H5Pset_deflate(voxel_cparms, compression);
    }


    // Create the extents dataset:
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
  void EventSparseTensor<dimension>::open_in_datasets(hid_t group){

    if (_open_in_datasets.size() < N_DATASETS ){
       _open_in_datasets.resize(N_DATASETS);
       _open_in_dataspaces.resize(N_DATASETS);
       

       _open_in_datasets[EXTENTS_DATASET]         = H5Dopen(group, "extents", H5P_DEFAULT);
       _open_in_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_in_datasets[EXTENTS_DATASET]);

       _open_in_datasets[VOXEL_EXTENTS_DATASET]   = H5Dopen(group, "voxel_extents", H5P_DEFAULT);
       _open_in_dataspaces[VOXEL_EXTENTS_DATASET] = H5Dget_space(_open_in_datasets[VOXEL_EXTENTS_DATASET]);

       _open_in_datasets[IMAGE_META_DATASET]      = H5Dopen(group, "image_meta", H5P_DEFAULT);
       _open_in_dataspaces[IMAGE_META_DATASET]    = H5Dget_space(_open_in_datasets[IMAGE_META_DATASET]);

       _open_in_datasets[VOXELS_DATASET]          = H5Dopen(group, "voxels", H5P_DEFAULT);
       _open_in_dataspaces[VOXELS_DATASET]        = H5Dget_space(_open_in_datasets[VOXELS_DATASET]);
;
     }

    return;
  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::open_out_datasets(hid_t group){

    if (_open_out_datasets.size() < N_DATASETS ){
       _open_out_datasets.resize(N_DATASETS);
       _open_out_dataspaces.resize(N_DATASETS);
       
       _open_out_datasets[EXTENTS_DATASET]         = H5Dopen(group,"extents", H5P_DEFAULT);
       _open_out_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);

       _open_out_datasets[VOXEL_EXTENTS_DATASET]   = H5Dopen(group,"voxel_extents", H5P_DEFAULT);
       _open_out_dataspaces[VOXEL_EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[VOXEL_EXTENTS_DATASET]);

       _open_out_datasets[IMAGE_META_DATASET]      = H5Dopen(group,"image_meta", H5P_DEFAULT);
       _open_out_dataspaces[IMAGE_META_DATASET]    = H5Dget_space(_open_out_datasets[IMAGE_META_DATASET]);

       _open_out_datasets[VOXELS_DATASET]          = H5Dopen(group,"voxels", H5P_DEFAULT);
       _open_out_dataspaces[VOXELS_DATASET]        = H5Dget_space(_open_out_datasets[VOXELS_DATASET]);
    }

    return;
  }

  template<size_t dimension>
  void EventSparseTensor<dimension>::finalize(){
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
  void EventSparseTensor<dimension>::serialize  (hid_t group){

    // Sparse tensors write one meta per projection ID.  So, the formatting of
    // the datasets is: 
    // Overall extents table indicates the right entries in the IDExtents table
    // IDExtents table contains a list of projection IDs in order, as well as the
    // corresponding voxel ranges in the Voxels table.
    // IDExtents is also one-to-one (and therefore mapped to by the extents table)
    // to a meta table.

    // Here is the order of operations for voxel serialization:
    // 1) Read the current dimensions of all tables (extents, voxel_extents, image_meta, voxels)
    // 2) Using the dimensions of the voxel table, build the voxel_extents object for this event
    // 2a) Using the same dimensions as 2), build the image_meta object for this event
    // 3) Using the dimensions of the voxel_extents table, and the dimensions of this event's voxel_extents, 
    //    update the extents table
    // 4) Update the voxel_extents table with the voxel_extents vector for this object.
    // 5) Update the image_meta table with the meta vector for this object.
    // 6) Update the voxels table with the voxels from this event, using the voxel_extents vector

    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);

    open_out_datasets(group);

    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////

    // Get the dataset current size
    hsize_t extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[EXTENTS_DATASET], extents_dims_current, NULL);

    // Get the dataset current size
    hsize_t voxel_extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[VOXEL_EXTENTS_DATASET], voxel_extents_dims_current, NULL);

    // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[IMAGE_META_DATASET], image_meta_dims_current, NULL);

    // Get the dataset current size
    hsize_t voxels_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[VOXELS_DATASET], voxels_dims_current, NULL);
    // std::cout << "Current extents size: " << extents_dims_current[0] << std::endl;
    // std::cout << "Current voxel_extents size: " << voxel_extents_dims_current[0] << std::endl;
    // std::cout << "Current voxels size: " << voxels_dims_current[0] << std::endl;



    /////////////////////////////////////////////////////////
    // Step 2: Build the voxel_extents object
    /////////////////////////////////////////////////////////

    // We need to make the voxel extents object first, which we can do from the vector of voxels.
    std::vector<IDExtents_t> voxel_extents;

    size_t last_voxel_index = voxels_dims_current[0];
    size_t n_new_voxels = 0;

    for (size_t projection_id = 0; projection_id < _tensor_v.size(); projection_id ++){
      voxel_extents.resize(voxel_extents.size() + 1);
      voxel_extents.back().n     = _tensor_v.at(projection_id).size();
      voxel_extents.back().id    = _tensor_v.at(projection_id).meta().projection_id();
      voxel_extents.back().first = last_voxel_index;
      last_voxel_index += voxel_extents.back().n;
      n_new_voxels += voxel_extents.back().n;
    }

    // std::cout << "Voxel Extents Size: " << voxel_extents.size() << std::endl;
    // for (auto & e : voxel_extents){
    //   std::cout << "  n: " << e.n << std::endl;
    // }


    /////////////////////////////////////////////////////////
    // Step 2a: Build the image_meta object
    /////////////////////////////////////////////////////////

    std::vector<ImageMeta<dimension> > image_meta;

    for (size_t projection_id = 0; projection_id < _tensor_v.size(); projection_id ++){
      image_meta.push_back(_tensor_v.at(projection_id).meta());
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
    H5Dset_extent(_open_out_datasets[EXTENTS_DATASET], extents_size);

    // Create an extents object to go into the extents table:

    Extents_t next_extents;
    next_extents.first = voxel_extents_dims_current[0];
    next_extents.n = voxel_extents.size();

    /////////////////////////////////////////////////////////
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_dims_current, // start
      NULL ,                // stride
      extents_slab_dims,    // count 
      NULL                  // block
    );

    // Define memory space:
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);


    // Write the new data
    H5Dwrite(_open_out_datasets[EXTENTS_DATASET],   // dataset_id,
             _data_types[EXTENTS_DATASET],          // hit_t mem_type_id, 
             extents_memspace,                      // hid_t mem_space_id, 
             _open_out_dataspaces[EXTENTS_DATASET], //hid_t file_space_id, 
             xfer_plist_id,                         //hid_t xfer_plist_id, 
             &(next_extents)                        // const void * buf 
           );


    /////////////////////////////////////////////////////////
    // Step 4: Update the voxel extents table
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t voxel_extents_slab_dims[1];
    voxel_extents_slab_dims[0] = voxel_extents.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t voxel_extents_size[1];
    voxel_extents_size[0] = voxel_extents_dims_current[0] + voxel_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[VOXEL_EXTENTS_DATASET], voxel_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new voxel_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[VOXEL_EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[VOXEL_EXTENTS_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[VOXEL_EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      voxel_extents_dims_current, // start
      NULL ,                      // stride
      voxel_extents_slab_dims,    // count 
      NULL                        // block
    );

    // Define memory space:
    hid_t voxel_extents_memspace = H5Screate_simple(1, voxel_extents_slab_dims, NULL);


    // Write the new data
    H5Dwrite(_open_out_datasets[VOXEL_EXTENTS_DATASET],   // dataset_id,
             _data_types[VOXEL_EXTENTS_DATASET],          // hit_t mem_type_id, 
             voxel_extents_memspace,                      // hid_t mem_space_id, 
             _open_out_dataspaces[VOXEL_EXTENTS_DATASET], // hid_t file_space_id, 
             xfer_plist_id,                               // hid_t xfer_plist_id, 
             &(voxel_extents[0])                          // const void * buf 
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

    for (size_t projection_id = 0; projection_id < _tensor_v.size(); projection_id ++){

        hsize_t new_voxels_slab_dims[1];
        hsize_t offset_voxels_slab_dims[1];

        new_voxels_slab_dims[0] = _tensor_v.at(projection_id).size();
        offset_voxels_slab_dims[0] = starting_index;

        // std::cout << "[" << projection_id << "][" << cluster_id << "]: \n"
        //           << "  Offset: " << offset_voxels_slab_dims[0]
        //           << "\n  starting_index: " << starting_index 
        //           << "\n  N: " << new_voxels_slab_dims[0]
        //           << "\n  N: " << _tensor_v.at(projection_id).at(cluster_id).size()
        //           << std::endl;

        // Select as a hyperslab the last section of data for writing:
        H5Sselect_hyperslab(_open_out_dataspaces[VOXELS_DATASET], 
          H5S_SELECT_SET, 
          offset_voxels_slab_dims, // start
          NULL ,                   // stride
          new_voxels_slab_dims,    // count 
          NULL                     // block
        );

        // Define memory space:
        hid_t voxels_memspace = H5Screate_simple(1, new_voxels_slab_dims, NULL);


        // Write the new data
        H5Dwrite(_open_out_datasets[VOXELS_DATASET],   // dataset_id,
                 _data_types[VOXELS_DATASET],          // hit_t mem_type_id, 
                 voxels_memspace,                      // hid_t mem_space_id, 
                 _open_out_dataspaces[VOXELS_DATASET], // hid_t file_space_id, 
                 xfer_plist_id,                        // hid_t xfer_plist_id, 
                 &(_tensor_v.at(projection_id).as_vector()[0]) // const void * buf 
               );


        starting_index += new_voxels_slab_dims[0];
    }




    return;

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::deserialize(hid_t group, size_t entry, bool reopen_groups){

    // This function reads in a set of voxels for either sparse tensors or sparse clusters
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the voxel_extents table information
    // 3) Use the entry information to get the image_meta table information
    // 4) Use the voxel_extents information to read the correct voxels
    // 5) Update the meta for each set correctly


    if (reopen_groups){
      _open_in_dataspaces.clear();
      _open_in_datasets.clear();
    }
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);

    open_in_datasets(group);
    /////////////////////////////////////////////////////////
    // Step 1: Get the extents information from extents dataset
    /////////////////////////////////////////////////////////

    // H5::DataSet * extents_dataset = &(_open_datasets[EXTENTS_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace extents_dataspace = extents_dataset->getSpace();


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;

    hsize_t extents_offset[1];
    extents_offset[0] = entry;

    /////////////////////////////////////////////////////////
    // Read this extents entry from the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset->getSpace();
    H5Sselect_hyperslab(_open_in_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_offset,    // start
      NULL ,             // stride
      extents_slab_dims, //count 
      NULL               // block
    );

    // Define memory space:
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);

    Extents_t input_extents;
    // Read the new data
    H5Dread(
      _open_in_datasets[EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                         // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(input_extents)                       // void * buf  OUT: Buffer to receive data read from file.
    );
    /////////////////////////////////////////////////////////
    // Step 2: Get the voxel_extents information
    /////////////////////////////////////////////////////////


    // Next, open the relevant sections of the data 

    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    // H5::DataSet * voxel_extents_dataset = &(_open_in_datasets[VOXEL_EXTENTS_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace voxel_extents_dataspace = voxel_extents_dataset->getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t voxel_extents_slab_dims[1];
    voxel_extents_slab_dims[0] = input_extents.n;

    hsize_t voxel_extents_offset[1];
    voxel_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    H5Sselect_hyperslab(_open_in_dataspaces[VOXEL_EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      voxel_extents_offset,    // start
      NULL ,                   // stride
      voxel_extents_slab_dims, // count 
      NULL                     // block
    );

    hid_t voxel_extents_memspace = H5Screate_simple(1, voxel_extents_slab_dims, NULL);

    std::vector<IDExtents_t> voxel_extents;

    // Reserve space for reading in voxel_extents:
    voxel_extents.resize(input_extents.n);

    H5Dread(
      _open_in_datasets[VOXEL_EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[VOXEL_EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      voxel_extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[VOXEL_EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                               // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(voxel_extents[0])                          // void * buf  OUT: Buffer to receive data read from file.
    );
    // std::cout << "voxel_extents.size(): " << voxel_extents.size() << std::endl;



    /////////////////////////////////////////////////////////
    // Step 3: Get the image_meta information
    /////////////////////////////////////////////////////////

    // H5::DataSet * image_meta_dataset = &(_open_in_datasets[IMAGE_META_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace image_meta_dataspace = image_meta_dataset->getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = input_extents.n;

    hsize_t image_meta_offset[1];
    image_meta_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    H5Sselect_hyperslab(_open_in_dataspaces[IMAGE_META_DATASET], 
      H5S_SELECT_SET, 
      image_meta_offset,    // start
      NULL ,                // stride
      image_meta_slab_dims, // count 
      NULL                  // block
    );

    hid_t image_meta_memspace = H5Screate_simple(1, image_meta_slab_dims, NULL);

    std::vector<larcv3::ImageMeta<dimension> > image_meta;

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
    // Step 4: Read the voxels
    /////////////////////////////////////////////////////////

    _tensor_v.clear();
    _tensor_v.resize(image_meta.size());

    size_t offset = voxel_extents.front().first;

    for (size_t voxel_set_index = 0; voxel_set_index < voxel_extents.size(); voxel_set_index ++){


      // // Make sure there is room for this:
      // if(_tensor_v.at(voxel_set_index).size() <= cluster_id){
      //   _tensor_v.at(voxel_set_index).as_vector().resize(cluster_id + 1);
      // }

      // Get the dataset for reading:
      // H5::DataSet * voxels_dataset = &(_open_in_datasets[VOXELS_DATASET]);

      // Get a dataspace inside this file:
      // H5::DataSpace voxels_dataspace = voxels_dataset->getSpace();

      // Create a dimension for the data to add (which is the hyperslab data)
      hsize_t voxels_slab_dims[1];
      voxels_slab_dims[0] = voxel_extents.at(voxel_set_index).n;

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
        _tensor_v.at(voxel_set_index).add(v);
      }
      _tensor_v.at(voxel_set_index).id(voxel_set_index);
      // std::cout << "_tensor_v.at(voxel_set_index).at(cluster_id).size(): " << _tensor_v.at(voxel_set_index).at(cluster_id).size() << std::endl;
    
      offset += voxels_slab_dims[0];

      // Set the meta for this object:
      _tensor_v.at(voxel_set_index).meta(image_meta.at(voxel_set_index));
    }



    return;

  }


template class EventSparseTensor<2>;
template class EventSparseTensor<3>;
}

#endif
