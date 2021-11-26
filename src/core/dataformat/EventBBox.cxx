#ifndef __LARCV3DATAFORMAT_EVENTBBOX_CXX
#define __LARCV3DATAFORMAT_EVENTBBOX_CXX

#include "larcv3/core/dataformat/EventBBox.h"

#define BBOX_EXTENTS_CHUNK_SIZE 1
#define BBOX_IDEXTENTS_CHUNK_SIZE 1
#define BBOX_CHUNK_SIZE 1000

#define BBOX_DATASET 0
#define EXTENTS_DATASET 1
#define BBOX_EXTENTS_DATASET 2
#define N_DATASETS 3


namespace larcv3 {

  static EventBBoxFactory<2> __global_EventBBox2DFactory__;
  static EventBBoxFactory<3> __global_EventBBox3DFactory__;

  template<size_t dimension>
  EventBBox<dimension>::EventBBox(){


    _data_types.resize(N_DATASETS);

    _data_types[EXTENTS_DATASET]       = larcv3::get_datatype<Extents_t>();
    _data_types[BBOX_EXTENTS_DATASET]  = larcv3::get_datatype<IDExtents_t>();
    _data_types[BBOX_DATASET]        = larcv3::BBox<dimension>::get_datatype();

  }

  template<size_t dimension>
  void EventBBox<dimension>::clear()
  {
    _bbox_c_v.clear();
  }


  template<size_t dimension>
  void EventBBox<dimension>::append(const BBoxCollection<dimension>& img)
  {
    _bbox_c_v.push_back(img);
  }

  template<size_t dimension>
  void EventBBox<dimension>::emplace_back(BBoxCollection<dimension>&& img)
  {
    _bbox_c_v.emplace_back(std::move(img));
  }

  template<size_t dimension>
  void EventBBox<dimension>::emplace(std::vector<larcv3::BBoxCollection<dimension>>&& image_v)
  {
    _bbox_c_v = std::move(image_v);
  }

  template<size_t dimension>
  void EventBBox<dimension>::set(const std::vector<larcv3::BBoxCollection<dimension>>& bbox_c_v){
    _bbox_c_v.clear();
    this -> _bbox_c_v = std::move(bbox_c_v);
  }


  template<size_t dimension>
  void EventBBox<dimension>::open_in_datasets(hid_t group){

    if (_open_in_datasets.size() < N_DATASETS ){
       _open_in_datasets.resize(N_DATASETS);
       _open_in_dataspaces.resize(N_DATASETS);

       _open_in_datasets[BBOX_DATASET]            = H5Dopen(group,"bboxes", H5P_DEFAULT);
       _open_in_dataspaces[BBOX_DATASET]          = H5Dget_space(_open_in_datasets[BBOX_DATASET]);

       _open_in_datasets[EXTENTS_DATASET]         = H5Dopen(group,"extents", H5P_DEFAULT);
       _open_in_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_in_datasets[EXTENTS_DATASET]);

       _open_in_datasets[BBOX_EXTENTS_DATASET]    = H5Dopen(group,"bbox_extents", H5P_DEFAULT);
       _open_in_dataspaces[BBOX_EXTENTS_DATASET]  = H5Dget_space(_open_in_datasets[BBOX_EXTENTS_DATASET]);

    }

    return;
  }

  template<size_t dimension>
  void EventBBox<dimension>::open_out_datasets(hid_t group){

    if (_open_out_datasets.size() < N_DATASETS ){
       _open_out_datasets.resize(N_DATASETS);
       _open_out_dataspaces.resize(N_DATASETS);

       _open_out_datasets[BBOX_DATASET]            = H5Dopen(group,"bboxes", H5P_DEFAULT);
       _open_out_dataspaces[BBOX_DATASET]          = H5Dget_space(_open_out_datasets[BBOX_DATASET]);

       _open_out_datasets[EXTENTS_DATASET]         = H5Dopen(group,"extents", H5P_DEFAULT);
       _open_out_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);

       _open_out_datasets[BBOX_EXTENTS_DATASET]    = H5Dopen(group,"bbox_extents", H5P_DEFAULT);
       _open_out_dataspaces[BBOX_EXTENTS_DATASET]  = H5Dget_space(_open_out_datasets[BBOX_EXTENTS_DATASET]);

    }

    return;
  }

  template<size_t dimension>
  void EventBBox<dimension>::finalize(){
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
  void EventBBox<dimension>::initialize (hid_t group, uint compression){

    // BBox creates a set of tables:
    // 1) extents: indicates which entries in the bbox_extents table correspond to the entry
    // 2) bbox_extents: indicates which entries in the bboxes table correspond to the entry
    // 3) bboxes: stores bboxes


    // Initialize is ONLY meant to be called on an empty group.  So, verify this group
    // is empty:

    // hsize_t  num_objects[1] = {9999};
    // H5Gget_num_objs(group, num_objects);
    if (get_num_objects(group) != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group << std::endl;
      throw larbys();
    }

    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // bbox_extents in the table.

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t extents_starting_dim[] = {0};
    hsize_t extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace
    hid_t extents_dataspace = H5Screate_simple(1, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t   extents_cparms        = H5Pcreate( H5P_DATASET_CREATE );
    hsize_t extents_chunk_dims[1] = {BBOX_EXTENTS_CHUNK_SIZE};
    H5Pset_chunk(extents_cparms, 1, extents_chunk_dims );
    if (compression){
      H5Pset_deflate(extents_cparms, compression);
    }

    hid_t lcpl = H5Pcreate(H5P_LINK_CREATE);
    hid_t dapl = H5Pcreate(H5P_DATASET_ACCESS);

    // Create the extents dataset:
    H5Dcreate(
      group,                        // hid_t loc_id  IN: Location identifier
      "extents",                    // const char *name      IN: Dataset name
      _data_types[EXTENTS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      extents_dataspace,            // hid_t space_id  IN: Dataspace identifier
      lcpl,                         // hid_t lcpl_id IN: Link creation property list
      extents_cparms,               // hid_t dcpl_id IN: Dataset creation property list
      dapl                          // hid_t dapl_id IN: Dataset access property list
    );

    /////////////////////////////////////////////////////////
    // Create the bbox_extents dataset
    /////////////////////////////////////////////////////////

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t bbox_extents_starting_dim[] = {0};
    hsize_t bbox_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace
    hid_t bbox_extents_dataspace = H5Screate_simple(1, bbox_extents_starting_dim, bbox_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t   bbox_extents_cparms         = H5Pcreate( H5P_DATASET_CREATE );
    hsize_t bbox_extents_chunk_dims[1]  = {BBOX_EXTENTS_CHUNK_SIZE};
    H5Pset_chunk(bbox_extents_cparms, 1, bbox_extents_chunk_dims );
    if (compression){
      H5Pset_deflate(bbox_extents_cparms, compression);
    }

    // Create the extents dataset:
    H5Dcreate(
      group,                             // hid_t loc_id  IN: Location identifier
      "bbox_extents",                    // const char *name      IN: Dataset name
      _data_types[BBOX_EXTENTS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      bbox_extents_dataspace,            // hid_t space_id  IN: Dataspace identifier
      lcpl,                              // hid_t lcpl_id IN: Link creation property list
      bbox_extents_cparms,               // hid_t dcpl_id IN: Dataset creation property list
      dapl                               // hid_t dapl_id IN: Dataset access property list
    );

    /////////////////////////////////////////////////////////
    // Create the bbox dataset
    /////////////////////////////////////////////////////////

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t bbox_starting_dim[] = {0};
    hsize_t bbox_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace
    hid_t bbox_dataspace = H5Screate_simple(1, bbox_starting_dim, bbox_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t   bbox_cparms         = H5Pcreate( H5P_DATASET_CREATE );
    hsize_t bbox_chunk_dims[1]  = {BBOX_CHUNK_SIZE};
    H5Pset_chunk(bbox_cparms, 1, bbox_chunk_dims );
    if (compression){
      H5Pset_deflate(bbox_cparms, compression);
    }

    // Create the extents dataset:
    H5Dcreate(
      group,                        // hid_t loc_id  IN: Location identifier
      "bboxes",                     // const char *name IN: Dataset name
      _data_types[BBOX_DATASET],    // hid_t dtype_id  IN: Datatype identifier
      bbox_dataspace,               // hid_t space_id  IN: Dataspace identifier
      lcpl,                         // hid_t lcpl_id IN: Link creation property list
      bbox_cparms,                  // hid_t dcpl_id IN: Dataset creation property list
      dapl                          // hid_t dapl_id IN: Dataset access property list
    );

    return;
  }

  template<size_t dimension>
  void EventBBox<dimension>::serialize(hid_t group){



    // Sparse cluster write one meta per projection ID.  So, the formatting of
    // the datasets is:
    // Overall extents table indicates the right entries (projection IDs) in the bboxes_extents table
    // bboxes_extents table contains a list of projection IDs in order, as well as the
    // corresponding bbox ranges in the Bboxes table.
    // Bboxes contains the bbox object

    // Here is the order of operations for voxel serialization:
    // 1) Read the current dimensions of all tables (extents, bbox_extents, bboxes)
    // 2) Using the dimensions of the bboxes table, build the bbox_extents object for this event
    // 3) Using the dimensions of the bbox_extents table, and the dimensions of this event's bbox_extents,
    //    update the extents table
    // 4) Update the bbox_extents table with the projection info for this event
    // 5) Update the bboxes table with the bboxes from this event, using the bboxes vector
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
    hsize_t bbox_extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[BBOX_EXTENTS_DATASET], bbox_extents_dims_current, NULL);

    // Get the dataset current size
    hsize_t bboxes_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[BBOX_DATASET], bboxes_dims_current, NULL);


    /////////////////////////////////////////////////////////
    // Step 2: Build the BBox Extents
    /////////////////////////////////////////////////////////


    // We need to make the image extents object first, which we can do from the vector of images.
    std::vector<IDExtents_t> bbox_extents;

    size_t last_bbox_index = bboxes_dims_current[0];
    size_t n_new_bbox_c    = _bbox_c_v.size();
    size_t new_bbox_c_size = 0;
    bbox_extents.resize(n_new_bbox_c);


    for (size_t bbox_collection_id = 0; bbox_collection_id < _bbox_c_v.size(); bbox_collection_id ++){
        bbox_extents[bbox_collection_id].n     = _bbox_c_v.at(bbox_collection_id).size();
        bbox_extents[bbox_collection_id].id    = bbox_collection_id;
        bbox_extents[bbox_collection_id].first = last_bbox_index;
        last_bbox_index += _bbox_c_v.at(bbox_collection_id).size();
        new_bbox_c_size += _bbox_c_v.at(bbox_collection_id).size();
    }

    /////////////////////////////////////////////////////////
    // Step 3: Update the extents table
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
    next_extents.first = bbox_extents_dims_current[0];
    next_extents.n = bbox_extents.size();



    _open_out_dataspaces[EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[EXTENTS_DATASET],
      H5S_SELECT_SET,
      extents_dims_current, // start
      NULL ,                // stride
      extents_slab_dims,    //count
      NULL                  // block
      );

    // Define memory space:
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);

    // Write the new data
    // _open_out_datasets[PARTICLES_DATASET].write(&(_part_v[0]), _data_types[PARTICLES_DATASET],
        // extents_memspace, _open_out_dataspaces[PARTICLES_DATASET]);

    H5Dwrite(_open_out_datasets[EXTENTS_DATASET],   // dataset_id,
             _data_types[EXTENTS_DATASET],          // hit_t mem_type_id,
             extents_memspace,                      // hid_t mem_space_id,
             _open_out_dataspaces[EXTENTS_DATASET], // hid_t file_space_id,
             xfer_plist_id,                         // hid_t xfer_plist_id,
             &(next_extents)                        // const void * buf
             );


    /////////////////////////////////////////////////////////
    // Step 4: Update the bbox_extents table
    /////////////////////////////////////////////////////////


    // BBox Extents to write is bbox_extents

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t bbox_extents_slab_dims[1];
    bbox_extents_slab_dims[0] = bbox_extents.size();

    // Create a size vector for the FULL dataset: previous + current
    hsize_t bbox_extents_size[1];
    bbox_extents_size[0] = bbox_extents_dims_current[0] + bbox_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[BBOX_EXTENTS_DATASET], bbox_extents_size);

    // Select a hyperslab to write to:
    _open_out_dataspaces[BBOX_EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[BBOX_EXTENTS_DATASET]);
    H5Sselect_hyperslab(_open_out_dataspaces[BBOX_EXTENTS_DATASET],
      H5S_SELECT_SET,
      bbox_extents_dims_current, // start
      NULL ,                     // stride
      bbox_extents_slab_dims,    // count
      NULL                       // block
      );

    // Define memory space:
    hid_t bbox_extents_memspace = H5Screate_simple(1, bbox_extents_slab_dims, NULL);

    // Write the new data
    // _open_out_datasets[PARTICLES_DATASET].write(&(_part_v[0]), _data_types[PARTICLES_DATASET],
        // bbox_extents_memspace, _open_out_dataspaces[PARTICLES_DATASET]);

    H5Dwrite(_open_out_datasets[BBOX_EXTENTS_DATASET],   // dataset_id,
             _data_types[BBOX_EXTENTS_DATASET],          // hit_t mem_type_id,
             bbox_extents_memspace,                      // hid_t mem_space_id,
             _open_out_dataspaces[BBOX_EXTENTS_DATASET], // hid_t file_space_id,
             xfer_plist_id,                              // hid_t xfer_plist_id,
             &(bbox_extents[0])                          // const void * buf
             );



    /////////////////////////////////////////////////////////
    // Write the new bboxes to the dataset
    /////////////////////////////////////////////////////////

    // How many BBoxes are we writing total?  It's in new_bbox_c_size

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t bbox_slab_dims[1];
    bbox_slab_dims[0] = new_bbox_c_size;

    // Create a size vector for the FULL dataset: previous + current
    hsize_t bbox_size[1];
    bbox_size[0] = bboxes_dims_current[0] + bbox_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[BBOX_DATASET], bbox_size);

    // Reopen the dataspace after extending:
    _open_out_dataspaces[BBOX_DATASET] = H5Dget_space(_open_out_datasets[BBOX_DATASET]);

    // We start writing after the last one:
    hsize_t starting_index = bboxes_dims_current[0];

    // Now we've extended the dataset.  Let's loop over projectionIDs and write to file:
    for (size_t projection_id = 0; projection_id < _bbox_c_v.size(); projection_id ++){


        hsize_t new_bboxes_slab_dims[1];
        hsize_t offset_bboxes_slab_dims[1];

        new_bboxes_slab_dims[0] = bbox_extents.at(projection_id).n;
        offset_bboxes_slab_dims[0] = starting_index;

        // std::cout << "[" << projection_id << "]: \n"
        //           << "  Offset: " << offset_bboxes_slab_dims[0]
        //           << "\n  starting_index: " << starting_index
        //           << "\n  N: " << new_bboxes_slab_dims[0]
        //           << "\n  N: " << _bbox_c_v.at(projection_id).size()
        //           << std::endl;

        // Select as a hyperslab the last section of data for writing:
        H5Sselect_hyperslab(
          _open_out_dataspaces[BBOX_DATASET],
          H5S_SELECT_SET,
          offset_bboxes_slab_dims, // start
          NULL ,                   // stride
          new_bboxes_slab_dims,    // count
          NULL                     // block
        );

        // Define memory space:
        hid_t bboxes_memspace = H5Screate_simple(1, new_bboxes_slab_dims, NULL);

        // Write the new data
        H5Dwrite(
          _open_out_datasets[BBOX_DATASET],             // dataset_id,
          _data_types[BBOX_DATASET],                    // hit_t mem_type_id,
          bboxes_memspace,                              // hid_t mem_space_id,
          _open_out_dataspaces[BBOX_DATASET],           // hid_t file_space_id,
          xfer_plist_id,                                // hid_t xfer_plist_id,
          &(_bbox_c_v.at(projection_id).as_vector()[0]) // const void * buf
        );
        starting_index += new_bboxes_slab_dims[0];

    }


    /////////////////////////////////////////////////////////
    // Serialized!
    /////////////////////////////////////////////////////////
  }


  template <size_t dimension>
  void EventBBox<dimension>::deserialize(hid_t group, size_t entry, bool reopen_groups){

    // Deserialization is, in some ways, easier than serialization.
    // We just have to read data from the file, and wrap it into an std::vector.


    if (reopen_groups){
      _open_in_dataspaces.clear();
      _open_in_datasets.clear();
    }

    open_in_datasets(group);

    // Transfer property list, default
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);


    // This function reads in a set of voxels for sparse clusters
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the bbox_extents table information
    // 3) Use the bbox_extents information to get the bboxes


    /////////////////////////////////////////////////////////
    // Step 1: Get the extents information from extents dataset
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;

    hsize_t extents_offset[1];
    extents_offset[0] = entry;


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
    // Step 2: Get the bbox_extents information
    /////////////////////////////////////////////////////////


    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t bbox_extents_slab_dims[1];
    bbox_extents_slab_dims[0] = input_extents.n;

    hsize_t bbox_extents_offset[1];
    bbox_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:

    H5Sselect_hyperslab(_open_in_dataspaces[BBOX_EXTENTS_DATASET],
      H5S_SELECT_SET,
      bbox_extents_offset,    // start
      NULL ,                  // stride
      bbox_extents_slab_dims, //count
      NULL                    // block
    );

    hid_t bbox_extents_memspace = H5Screate_simple(1, bbox_extents_slab_dims, NULL);


    std::vector<IDExtents_t> bbox_extents;

    // Reserve space for reading in bbox_extents:
    bbox_extents.resize(input_extents.n);

    H5Dread(
      _open_in_datasets[BBOX_EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[BBOX_EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      bbox_extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[BBOX_EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                              // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(bbox_extents[0])                          // void * buf  OUT: Buffer to receive data read from file.
    );


    /////////////////////////////////////////////////////////
    // Step 3: Read the bboxes
    /////////////////////////////////////////////////////////

    // At this point, we know the following:
    // - How many projections there are (bbox_extents.size())
    //
    // To make things useful, we untangle the the clusters per projection

    _bbox_c_v.clear();
    _bbox_c_v.resize(bbox_extents.size());

    // Offset of the first read
    hsize_t offset = bbox_extents.front().first;

    for (size_t projection_id = 0; projection_id < bbox_extents.size(); projection_id ++){

         // Make space for bboxes:
        _bbox_c_v.at(projection_id).resize(bbox_extents.at(projection_id).n);

        // Create a dimension for the data to add (which is the hyperslab data)
        hsize_t bbox_slab_dims[1];
        bbox_slab_dims[0] = bbox_extents.at(projection_id).n;

        hsize_t bbox_offset[1];
        bbox_offset[0] = offset;

        // std::cout << "N: " << voxels_slab_dims[0] << "\n"
        //           << "offset: " << bbox_offset[0] << "\n"
        //           << std::endl;
        // Now, select as a hyperslab the last section of data for readomg:
        H5Sselect_hyperslab(_open_in_dataspaces[BBOX_DATASET],
          H5S_SELECT_SET,
          bbox_offset,      // start
          NULL ,            // stride
          bbox_slab_dims,   // count
          NULL              // block
        );

        hid_t bbox_memspace = H5Screate_simple(1, bbox_slab_dims, NULL);

        // Reserve space for reading in voxels:

        H5Dread(
          _open_in_datasets[BBOX_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
          _data_types[BBOX_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
          bbox_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
          _open_in_dataspaces[BBOX_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
          xfer_plist_id,                      // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
          &(_bbox_c_v[projection_id]._bbox_v[0])      // void * buf  OUT: Buffer to receive data read from file.
        );

        offset += bbox_slab_dims[0];
    }

    return;


  }



  template class EventBBox<2>;
  template class EventBBox<3>;


  template<> std::string as_string<EventBBox<2>>() {return "EventBBox2D";}
  template<> std::string as_string<EventBBox<3>>() {return "EventBBox3D";}


}

#include <pybind11/stl.h>

template <size_t dimension>
void init_eventbbox_instance(pybind11::module m){

  using Class = larcv3::EventBBox<dimension>;
  pybind11::class_<Class, std::shared_ptr<Class>> event_bbox(m, larcv3::as_string<Class>().c_str());
  event_bbox.def(pybind11::init<>());

  event_bbox.def("set",             &Class::set);
  event_bbox.def("append",          &Class::append);
  // event_bbox.def("emplace_back",    &Class::emplace_back);
  // event_bbox.def("emplace",         &Class::emplace);
  event_bbox.def("at",              &Class::at);
  event_bbox.def("as_vector",       &Class::as_vector);
  event_bbox.def("size",            &Class::size);
  event_bbox.def("clear",           &Class::clear);

/*



    static EventParticle * to_particle(EventBase * e){
      return (EventParticle *) e;
    }

*/

}


void init_eventbbox(pybind11::module m){
  init_eventbbox_instance<2>(m);
  init_eventbbox_instance<3>(m);
}


#endif


