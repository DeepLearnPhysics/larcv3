#ifndef __LARCV3DATAFORMAT_EVENTTENSOR_CXX
#define __LARCV3DATAFORMAT_EVENTTENSOR_CXX

#include "larcv3/core/dataformat/EventTensor.h"
// #include "larcv3/core/Base/larbys.h"

#define IMAGE_EXTENTS_CHUNK_SIZE 1
#define IMAGE_IDEXTENTS_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 1000

#define IMAGES_DATASET 0
#define EXTENTS_DATASET 1
#define IMAGE_META_DATASET 2
#define IMAGE_EXTENTS_DATASET 3
#define N_DATASETS 4

namespace larcv3 {

  /// Global larcv3::SBClusterFactory to register ClusterAlgoFactory
  static EventTensorFactory<1> __global_EventTensor1DFactory__;
  static EventTensorFactory<2> __global_EventTensor2DFactory__;
  static EventTensorFactory<3> __global_EventTensor3DFactory__;
  static EventTensorFactory<4> __global_EventTensor4DFactory__;

  template<size_t dimension> 
  EventTensor<dimension>::EventTensor(){


    _data_types.resize(N_DATASETS);

    _data_types[EXTENTS_DATASET]       = larcv3::get_datatype<Extents_t>();
    _data_types[IMAGE_EXTENTS_DATASET] = larcv3::get_datatype<IDExtents_t>();
    _data_types[IMAGE_META_DATASET]    = larcv3::ImageMeta<dimension>::get_datatype();
    _data_types[IMAGES_DATASET]        = larcv3::get_datatype<float>();


  }

  template<size_t dimension> 
  void EventTensor<dimension>::clear()
  {
    _image_v.clear();
  }


  template<size_t dimension> 
  void EventTensor<dimension>::append(const Tensor<dimension>& img)
  {
    _image_v.push_back(img);
  }

  template<size_t dimension> 
  void EventTensor<dimension>::emplace(Tensor<dimension>&& img)
  {
    _image_v.emplace_back(std::move(img));
  }

  template<size_t dimension> 
  void EventTensor<dimension>::emplace(std::vector<larcv3::Tensor<dimension>>&& image_v)
  {
    _image_v = std::move(image_v);
  }


  template<size_t dimension> 
  void EventTensor<dimension>::open_in_datasets(hid_t group){

    if (_open_in_datasets.size() < N_DATASETS ){
       _open_in_datasets.resize(N_DATASETS);
       _open_in_dataspaces.resize(N_DATASETS);
       
       _open_in_datasets[IMAGES_DATASET]          = H5Dopen(group,"images", H5P_DEFAULT);
       _open_in_dataspaces[IMAGES_DATASET]        = H5Dget_space(_open_in_datasets[IMAGES_DATASET]);

       _open_in_datasets[EXTENTS_DATASET]         = H5Dopen(group,"extents", H5P_DEFAULT);
       _open_in_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_in_datasets[EXTENTS_DATASET]);

       _open_in_datasets[IMAGE_META_DATASET]      = H5Dopen(group,"image_meta", H5P_DEFAULT);
       _open_in_dataspaces[IMAGE_META_DATASET]    = H5Dget_space(_open_in_datasets[IMAGE_META_DATASET]);

       _open_in_datasets[IMAGE_EXTENTS_DATASET]   = H5Dopen(group,"image_extents", H5P_DEFAULT);
       _open_in_dataspaces[IMAGE_EXTENTS_DATASET] = H5Dget_space(_open_in_datasets[IMAGE_EXTENTS_DATASET]);

    }

    return;
  }

  template<size_t dimension> 
  void EventTensor<dimension>::open_out_datasets(hid_t group){

    if (_open_out_datasets.size() < N_DATASETS ){
       _open_out_datasets.resize(N_DATASETS);
       _open_out_dataspaces.resize(N_DATASETS);
       
       _open_out_datasets[IMAGES_DATASET]          = H5Dopen(group,"images", H5P_DEFAULT);
       _open_out_dataspaces[IMAGES_DATASET]        = H5Dget_space(_open_out_datasets[IMAGES_DATASET]);

       _open_out_datasets[EXTENTS_DATASET]         = H5Dopen(group,"extents", H5P_DEFAULT);
       _open_out_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);


       _open_out_datasets[IMAGE_META_DATASET]      = H5Dopen(group,"image_meta", H5P_DEFAULT);
       _open_out_dataspaces[IMAGE_META_DATASET]    = H5Dget_space(_open_out_datasets[IMAGE_META_DATASET]);

       _open_out_datasets[IMAGE_EXTENTS_DATASET]   = H5Dopen(group,"image_extents", H5P_DEFAULT);
       _open_out_dataspaces[IMAGE_EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[IMAGE_EXTENTS_DATASET]);

    }

    return;
  }

  template<size_t dimension>
  void EventTensor<dimension>::finalize(){
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
  void EventTensor<dimension>::initialize (hid_t group, uint compression){

    // Image2D creates a set of tables:
    // 1) extents: indicates which entries in the image_extents table correspond to the entry
    // 2) image_extents: indicates which entries in the images and image_metas table correspond to the entry
    // 3) images: stores images
    // 4) image_metas: stores image meta information.


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
    // particles in the data tree.

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t extents_starting_dim[] = {0};
    hsize_t extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t extents_dataspace = H5Screate_simple(1, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    hsize_t      extents_chunk_dims[1] ={IMAGE_EXTENTS_CHUNK_SIZE};
    H5Pset_chunk(extents_cparms, 1, extents_chunk_dims );
    if (compression){
      H5Pset_deflate(extents_cparms, compression);
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

    std::cout << "Created extents " << std::endl;
    /////////////////////////////////////////////////////////
    // Create the image_extents dataset
    /////////////////////////////////////////////////////////

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t image_extents_starting_dim[] = {0};
    hsize_t image_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t image_extents_dataspace = H5Screate_simple(1, image_extents_starting_dim, image_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t image_extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    hsize_t      image_extents_chunk_dims[1] ={IMAGE_IDEXTENTS_CHUNK_SIZE};
    H5Pset_chunk(image_extents_cparms, 1, image_extents_chunk_dims );
    if (compression){
      H5Pset_deflate(image_extents_cparms, compression);
    }

    // Create the extents dataset:
    H5Dcreate(
      group,                              // hid_t loc_id  IN: Location identifier
      "image_extents",                    // const char *name      IN: Dataset name
      _data_types[IMAGE_EXTENTS_DATASET], // hid_t dtype_id  IN: Datatype identifier
      image_extents_dataspace,            // hid_t space_id  IN: Dataspace identifier
      NULL,                               // hid_t lcpl_id IN: Link creation property list
      image_extents_cparms,               // hid_t dcpl_id IN: Dataset creation property list
      NULL                                // hid_t dapl_id IN: Dataset access property list
    );
    std::cout << "Created image extents " << std::endl;

    /////////////////////////////////////////////////////////
    // Create the image_meta dataset
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    // H5::DataType image_meta_datatype = larcv3::ImageMeta2D::get_datatype();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t image_meta_starting_dim[] = {0};
    hsize_t image_meta_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t image_meta_dataspace = H5Screate_simple(1, image_meta_starting_dim, image_meta_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t image_meta_cparms = H5Pcreate( H5P_DATASET_CREATE );
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

    _compression = compression;

    std::cout << "Initialized" << std::endl;

    return;
  }
  template<size_t dimension> 
  void EventTensor<dimension>::serialize  (hid_t group){

    // This is something of an optimization trickery.
    // The dataset for storing images is not created until an image is written.
    // This enables us to look at the incoming data and set the chunk size

    /////////////////////////////////////////////////////////
    // Create the Image dataset
    /////////////////////////////////////////////////////////
    // if ( ! group -> nameExists("images")){
    if (  get_num_objects(group) != 4){
        // std::cout << "Images dataset does not yet exist, creating it." << std::endl;
        // An image is stored as a flat vector, so it's type is float.
        // The image ID is store in the image_extents table, and the meta in the image_meta table


        // Get the starting size (0) and dimensions (unlimited)
        hsize_t image_starting_dim[] = {0};
        hsize_t image_maxsize_dim[]  = {H5S_UNLIMITED};

        // Create a dataspace 
        hid_t image_dataspace = H5Screate_simple(1, image_starting_dim, image_maxsize_dim);

        // Figure out the chunk size dynamically:
        size_t chunk_size = 0;
        for (auto & image : _image_v){
            chunk_size += image.size();
        }

        /*
         * Modify dataset creation properties, i.e. enable chunking.
         */
        hid_t image_cparms = H5Pcreate( H5P_DATASET_CREATE );
        hsize_t      image_chunk_dims[1] ={chunk_size};
        H5Pset_chunk(image_cparms, 1, image_chunk_dims );
        if (_compression){
          H5Pset_deflate(image_cparms, _compression);
        }
        // Create the extents dataset:
        H5Dcreate(
          group,                           // hid_t loc_id  IN: Location identifier
          "images",                    // const char *name      IN: Dataset name
          _data_types[IMAGES_DATASET], // hid_t dtype_id  IN: Datatype identifier
          image_dataspace,            // hid_t space_id  IN: Dataspace identifier
          NULL,                            // hid_t lcpl_id IN: Link creation property list
          image_cparms,               // hid_t dcpl_id IN: Dataset creation property list
          NULL                             // hid_t dapl_id IN: Dataset access property list
        );


    }
    
    open_out_datasets(group);


    // Serialization of images proceeds as:
    // 1) Read the current dimensions of all tables (extents, image_extents, image_meta, images)
    // 2) Using the dimensions of the image table, build the image_extents object for this event
    // 3) Using the dimensions of the image_extents object, build the image_meta object for this event
    // 4) Using the dimensions of the image_extents table, and the dimensions of this event's image_extents, 
    //    update the extents table
    // 5) Update the image_extents table with the image_extents vector for this object.
    // 6) Update the image_meta table with the image_meta vector for this object.
    // 7) Update the images table with the images from this event, using the image_extents vector



    // Transfer property list, default:
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);


    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////

    // Get the dataset current size
    hsize_t extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[EXTENTS_DATASET], extents_dims_current, NULL);


    // Get the dataset current size
    hsize_t image_extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[IMAGE_EXTENTS_DATASET], image_extents_dims_current, NULL);

        // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[IMAGE_META_DATASET], image_meta_dims_current, NULL);


    // Get the dataset current size
    hsize_t images_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[IMAGES_DATASET], images_dims_current, NULL);


    /////////////////////////////////////////////////////////
    // Step 2: Build the image_extents object
    /////////////////////////////////////////////////////////

    // We need to make the image extents object first, which we can do from the vector of images.
    std::vector<IDExtents_t> image_extents;

    size_t last_image_index = images_dims_current[0];
    size_t n_new_images = _image_v.size();
    size_t extents_offset = image_extents.size();
    size_t new_image_size = 0;
    image_extents.resize(extents_offset + n_new_images);


    for (size_t image_id = 0; image_id < _image_v.size(); image_id ++){
        image_extents[image_id].n     = _image_v.at(image_id).size();
        image_extents[image_id].id    = _image_v.at(image_id).meta().id();
        image_extents[image_id].first = last_image_index;
        last_image_index += _image_v.at(image_id).size();
        new_image_size +=  _image_v.at(image_id).size();


    }



    /////////////////////////////////////////////////////////
    // Step 3: Build the image_meta object
    /////////////////////////////////////////////////////////
    std::vector<ImageMeta<dimension>> image_meta;
    image_meta.resize(n_new_images);
    for (size_t image_id = 0; image_id < _image_v.size(); image_id ++){
      image_meta.at(image_id) = _image_v.at(image_id).meta();
    }


    /////////////////////////////////////////////////////////
    // Step 4: Update the overall extents table
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
    next_extents.first = image_extents_dims_current[0];
    next_extents.n = image_extents.size();

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
    // Step 5: Update the image extents table
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_extents_slab_dims[1];
    image_extents_slab_dims[0] = image_extents.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t image_extents_size[1];
    image_extents_size[0] = image_extents_dims_current[0] + image_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[IMAGE_EXTENTS_DATASET], image_extents_size);



    /////////////////////////////////////////////////////////
    // Write the new image_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[IMAGE_EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[IMAGE_EXTENTS_DATASET]);

    H5Sselect_hyperslab(_open_out_dataspaces[IMAGE_EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      image_extents_dims_current, // start
      NULL ,                      // stride
      image_extents_slab_dims,    // count 
      NULL                        // block
      );
    // Define memory space:
    hid_t image_extents_memspace = H5Screate_simple(1, image_extents_slab_dims, NULL);


    // Write the new data
    H5Dwrite(_open_out_datasets[IMAGE_EXTENTS_DATASET],   // dataset_id,
             _data_types[IMAGE_EXTENTS_DATASET],          // hit_t mem_type_id, 
             image_extents_memspace,                      // hid_t mem_space_id, 
             _open_out_dataspaces[IMAGE_EXTENTS_DATASET], //hid_t file_space_id, 
             xfer_plist_id,                               //hid_t xfer_plist_id, 
             &(image_extents[0])                          // const void * buf 
           );

    /////////////////////////////////////////////////////////
    // Step 6: Update the image meta table
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = image_meta.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t image_meta_size[1];
    image_meta_size[0] = image_meta_dims_current[0] + image_meta_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[IMAGE_META_DATASET], image_meta_size);



    /////////////////////////////////////////////////////////
    // Write the new image_meta to the dataset
    /////////////////////////////////////////////////////////

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
             _open_out_dataspaces[IMAGE_META_DATASET], //hid_t file_space_id, 
             xfer_plist_id,                            //hid_t xfer_plist_id, 
             &(image_meta[0])                          // const void * buf 
           );


    /////////////////////////////////////////////////////////
    // Step 7: Write new images
    /////////////////////////////////////////////////////////




    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t images_slab_dims[1];
    images_slab_dims[0] = new_image_size;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t images_size[1];
    images_size[0] = images_dims_current[0] + images_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[IMAGES_DATASET], images_size);

    _open_out_dataspaces[IMAGES_DATASET] = H5Dget_space(_open_out_datasets[IMAGES_DATASET]);


    // Write all of the images to file:

    // Keep track of starting index for a write:
    size_t starting_index = images_dims_current[0];

    for (size_t image_id = 0; image_id < _image_v.size(); image_id ++){

        hsize_t new_images_slab_dims[1];
        hsize_t offset_images_slab_dims[1];

        new_images_slab_dims[0] = _image_v.at(image_id).size();
        offset_images_slab_dims[0] = starting_index;

        // std::cout << "[" << image_id << "][" << cluster_id << "]: \n"
        //           << "  Offset: " << offset_images_slab_dims[0]
        //           << "\n  starting_index: " << starting_index 
        //           << "\n  N: " << new_images_slab_dims[0]
        //           << "\n  N: " << _image_v.at(image_id).at(cluster_id).size()
        //           << std::endl;

        // Select as a hyperslab the last section of data for writing:
        H5Sselect_hyperslab(_open_out_dataspaces[IMAGES_DATASET], 
              H5S_SELECT_SET, 
              offset_images_slab_dims, // start
              NULL ,                   // stride
              new_images_slab_dims,    // count 
              NULL                     // block
            );

        // Define memory space:
        hid_t images_memspace = H5Screate_simple(1, new_images_slab_dims, NULL);


        // Write the new data
        H5Dwrite(_open_out_datasets[IMAGES_DATASET],   // dataset_id,
                 _data_types[IMAGES_DATASET],          // hit_t mem_type_id, 
                 images_memspace,                      // hid_t mem_space_id, 
                 _open_out_dataspaces[IMAGES_DATASET], //hid_t file_space_id, 
                 xfer_plist_id,                            //hid_t xfer_plist_id, 
                 &(_image_v.at(image_id).as_vector()[0])                          // const void * buf 
               );
        starting_index += new_images_slab_dims[0];
    }







    return;
  }
  template<size_t dimension> 
  void EventTensor<dimension>::deserialize(hid_t group, size_t entry, bool reopen_groups){
   
    // This function reads in a set of images
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the image_extents table information
    // 3) Use the entry table to get the image_meta information
    // 4) Read the image_meta and use it to build the _image_v vector with correct image sizes
    // 3) Read the images directly into the correct memory locations


    if (reopen_groups){
      _open_in_dataspaces.clear();
      _open_in_datasets.clear();
    }
    
    open_in_datasets(group);


    // Transfer property list, default
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);

    /////////////////////////////////////////////////////////
    // Step 1: Get the extents information from extents dataset
    /////////////////////////////////////////////////////////

    // H5::DataSet extents_dataset = group->openDataSet("extents");

    // Get a dataspace inside this file:
    // H5::DataSpace extents_dataspace = extents_dataset.getSpace();


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;

    hsize_t extents_offset[1];
    extents_offset[0] = entry;

    /////////////////////////////////////////////////////////
    // Read this extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    H5Sselect_hyperslab(_open_in_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_offset,     // start
      NULL ,              // stride
      extents_slab_dims,  // count 
      NULL                // block
    );

    // Define memory space:
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);

    Extents_t input_extents;
    // Read the data
    H5Dread(
      _open_in_datasets[EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                         // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(input_extents)                       // void * buf  OUT: Buffer to receive data read from file.
    );
    /////////////////////////////////////////////////////////
    // Step 2: Get the image_extents information
    /////////////////////////////////////////////////////////


    // Next, open the relevant sections of the data 

    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    // H5::DataSet image_extents_dataset = group->openDataSet("image_extents");

    // Get a dataspace inside this file:
    // H5::DataSpace image_extents_dataspace = _open_in_datasets[IMAGE_EXTENTS_DATASET].getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_extents_slab_dims[1];
    image_extents_slab_dims[0] = input_extents.n;

    hsize_t image_extents_offset[1];
    image_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    H5Sselect_hyperslab(_open_in_dataspaces[IMAGE_EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      image_extents_offset, // start
      NULL ,  // stride
      image_extents_slab_dims, //count 
      NULL // block
    );
  

    hid_t image_extents_memspace = H5Screate_simple(1, image_extents_slab_dims, NULL);

    std::vector<IDExtents_t> image_extents;

    // Reserve space for reading in image_extents:
    image_extents.resize(input_extents.n);
    H5Dread(
      _open_in_datasets[IMAGE_EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[IMAGE_EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      image_extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[IMAGE_EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                               // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(image_extents[0])                          // void * buf  OUT: Buffer to receive data read from file.
    );


    /////////////////////////////////////////////////////////
    // Step 3: Get the image_meta information
    /////////////////////////////////////////////////////////


    // H5::DataSet image_meta_dataset = group->openDataSet("image_meta");

    // Get a dataspace inside this file:
    // H5::DataSpace image_meta_dataspace = image_meta_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = input_extents.n;

    hsize_t image_meta_offset[1];
    image_meta_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    H5Sselect_hyperslab(_open_in_dataspaces[IMAGE_META_DATASET], 
      H5S_SELECT_SET, 
      image_meta_offset, // start
      NULL ,  // stride
      image_meta_slab_dims, //count 
      NULL // block
    );

    hid_t image_meta_memspace = H5Screate_simple(1, image_meta_slab_dims, NULL);

    std::vector<ImageMeta<dimension>> image_meta;

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
      


    /////////////////////////////////////////////////////////
    // Step 4: Allocate the memory for the images:
    /////////////////////////////////////////////////////////


    _image_v.clear();
    for (size_t image_index = 0; image_index < image_meta.size(); image_index ++){
      _image_v.push_back(Tensor<dimension>(image_meta.at(image_index)));
    }

    /////////////////////////////////////////////////////////
    // Step 3: Read the images
    /////////////////////////////////////////////////////////


    size_t offset = image_extents.front().first;

    for (size_t image_index = 0; image_index < image_meta.size(); image_index ++){


      // Get the dataset for reading:
      // H5::DataSet images_dataset = group->openDataSet("images");

      // Get a dataspace inside this file:
      // H5::DataSpace images_dataspace = images_dataset.getSpace();

      // Create a dimension for the data to add (which is the hyperslab data)
      hsize_t images_slab_dims[1];
      images_slab_dims[0] = image_extents.at(image_index).n;

      hsize_t images_offset[1];
      images_offset[0] = offset;

      // std::cout << "N: " << images_slab_dims[0] << "\n"
      //           << "offset: " << images_offset[0] << "\n"
      //           << std::endl;
      // Now, select as a hyperslab the last section of data for readomg:
      H5Sselect_hyperslab(_open_in_dataspaces[IMAGES_DATASET], 
        H5S_SELECT_SET, 
        images_offset, // start
        NULL ,  // stride
        images_slab_dims, //count 
        NULL // block
      );

      hid_t images_memspace = H5Screate_simple(1, images_slab_dims, NULL);


      H5Dread(
        _open_in_datasets[IMAGES_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
        _data_types[IMAGES_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
        images_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
        _open_in_dataspaces[IMAGES_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
        xfer_plist_id,                            // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
        &(_image_v[image_index]._img[0])                          // void * buf  OUT: Buffer to receive data read from file.
      );


      offset += images_slab_dims[0];
    }


    return;
  }

  template class EventTensor<1>;
  template class EventTensor<2>;
  template class EventTensor<3>;
  template class EventTensor<4>;

}

#endif
