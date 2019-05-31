#ifndef __LARCV3DATAFORMAT_EVENTIMAGE2D_CXX
#define __LARCV3DATAFORMAT_EVENTIMAGE2D_CXX

#include "larcv3/core/dataformat/EventImage2D.h"
// #include "larcv3/core/Base/larbys.h"

#define IMAGE_EXTENTS_CHUNK_SIZE 1
#define IMAGE_IDEXTENTS_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 1000
#define IMAGE_COMPRESSION_LEVEL 1

namespace larcv3 {

  /// Global larcv3::SBClusterFactory to register ClusterAlgoFactory
  static EventImage2DFactory __global_EventImage2DFactory__;

  void EventImage2D::clear()
  {
    _image_v.clear();
  }


  void EventImage2D::append(const Image2D& img)
  {
    _image_v.push_back(img);
  }

  void EventImage2D::emplace(Image2D&& img)
  {
    _image_v.emplace_back(std::move(img));
  }

  void EventImage2D::emplace(std::vector<larcv3::Image2D>&& image_v)
  {
    _image_v = std::move(image_v);
  }


  void EventImage2D::open_datasets(H5::Group * group){

    // if (_open_datasets.size() < N_DATASETS ){
    //     std::cout << "Opening datasets" << std::endl;
    //    _open_datasets.resize(N_DATASETS);
    //    _open_dataspaces.resize(N_DATASETS);
       
    //    _open_datasets[EXTENTS_DATASET]         = group->openDataSet("extents");
    //    _open_dataspaces[EXTENTS_DATASET]       = _open_datasets[EXTENTS_DATASET].getSpace();

    //    _open_datasets[PROJECTION_DATASET]         = group->openDataSet("projection");
    //    _open_dataspaces[PROJECTION_DATASET]       = _open_datasets[PROJECTION_DATASET].getSpace();


    //    _open_datasets[CLUSTER_EXTENTS_DATASET]   = group->openDataSet("cluster_extents");
    //    _open_dataspaces[CLUSTER_EXTENTS_DATASET] = _open_datasets[CLUSTER_EXTENTS_DATASET].getSpace();

    //    _open_datasets[IMAGE_META_DATASET]      = group->openDataSet("image_meta");
    //    _open_dataspaces[IMAGE_META_DATASET]    = _open_datasets[IMAGE_META_DATASET].getSpace();

    //    _open_datasets[VOXELS_DATASET]          = group->openDataSet("voxels");
    //    _open_dataspaces[VOXELS_DATASET]        = _open_datasets[VOXELS_DATASET].getSpace();

    // }

    return;
  }

  void EventImage2D::initialize (H5::Group * group){

    // Image2D creates a set of tables:
    // 1) extents: indicates which entries in the image_extents table correspond to the entry
    // 2) image_extents: indicates which entries in the images and image_metas table correspond to the entry
    // 3) images: stores images
    // 4) image_metas: stores image meta information.


       // Initialize is ONLY meant to be called on an empty group.  So, verify this group 
    // is empty:

    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty image2d group " << group->fromClass() << std::endl;
      throw larbys();
    }

    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // particles in the data tree.


    // Get the data type for extents:
    H5::DataType extents_datatype = larcv3::get_datatype<Extents_t>();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t extents_starting_dim[] = {0};
    hsize_t extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace extents_dataspace(1, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList extents_cparms;
    hsize_t      extents_chunk_dims[1] ={IMAGE_EXTENTS_CHUNK_SIZE};
    extents_cparms.setChunk( 1, extents_chunk_dims );
    extents_cparms.setDeflate(IMAGE_COMPRESSION_LEVEL);
    
    // Create the extents dataset:
    H5::DataSet extents_ds = group->createDataSet("extents", extents_datatype, extents_dataspace, extents_cparms);


    /////////////////////////////////////////////////////////
    // Create the image_extents dataset
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    H5::DataType image_extents_datatype = larcv3::get_datatype<IDExtents_t>();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t image_extents_starting_dim[] = {0};
    hsize_t image_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace image_extents_dataspace(1, image_extents_starting_dim, image_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList image_extents_cparms;
    hsize_t      image_extents_chunk_dims[1] ={IMAGE_IDEXTENTS_CHUNK_SIZE};
    image_extents_cparms.setChunk( 1, image_extents_chunk_dims );
    image_extents_cparms.setDeflate(IMAGE_COMPRESSION_LEVEL);
    
    // Create the extents dataset:
    H5::DataSet image_extents_ds = group->createDataSet("image_extents", 
      image_extents_datatype, image_extents_dataspace, image_extents_cparms);


    /////////////////////////////////////////////////////////
    // Create the image_meta dataset
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    H5::DataType image_meta_datatype = larcv3::ImageMeta2D::get_datatype();

    // Get the starting size (0) and dimensions (unlimited)
    hsize_t image_meta_starting_dim[] = {0};
    hsize_t image_meta_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace image_meta_dataspace(1, image_meta_starting_dim, image_meta_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList image_meta_cparms;
    hsize_t      image_meta_chunk_dims[1] ={IMAGE_META_CHUNK_SIZE};
    image_meta_cparms.setChunk( 1, image_meta_chunk_dims );
    image_meta_cparms.setDeflate(IMAGE_COMPRESSION_LEVEL);

    // Create the extents dataset:
    H5::DataSet image_meta_ds = group->createDataSet("image_meta", 
      image_meta_datatype, image_meta_dataspace, image_meta_cparms);


    
    return;
  }
  void EventImage2D::serialize  (H5::Group * group){


    // This is something of an optimization trickery.
    // The dataset for storing images is not created until an image is written.
    // This enables us to look at the incoming data and set the chunk size


    /////////////////////////////////////////////////////////
    // Create the Image dataset
    /////////////////////////////////////////////////////////
    // if ( ! group -> nameExists("images")){
    if (  group -> getNumObjs() != 4){
        // std::cout << "Images dataset does not yet exist, creating it." << std::endl;
        // An image is stored as a flat vector, so it's type is float.
        // The image ID is store in the image_extents table, and the meta in the image_meta table

        // Get the data type for extents:
        H5::DataType image_datatype = larcv3::get_datatype<float>();


        // Get the starting size (0) and dimensions (unlimited)
        hsize_t image_starting_dim[] = {0};
        hsize_t image_maxsize_dim[]  = {H5S_UNLIMITED};

        // Create a dataspace 
        H5::DataSpace image_dataspace(1, image_starting_dim, image_maxsize_dim);

        // Figure out the chunk size dynamically:
        size_t chunk_size = 0;
        for (auto & image : _image_v){
            chunk_size += image.size();
        }

        /*
         * Modify dataset creation properties, i.e. enable chunking.
         */
        H5::DSetCreatPropList image_cparms;
        hsize_t      image_chunk_dims[1] ={chunk_size};
        image_cparms.setChunk( 1, image_chunk_dims );
        image_cparms.setDeflate(IMAGE_COMPRESSION_LEVEL);
        // Create the extents dataset:
        H5::DataSet image_ds = group->createDataSet("images", image_datatype, image_dataspace, image_cparms);



    }
    


    // Serialization of images proceeds as:
    // 1) Read the current dimensions of all tables (extents, image_extents, image_meta, images)
    // 2) Using the dimensions of the image table, build the image_extents object for this event
    // 3) Using the dimensions of the image_extents object, build the image_meta object for this event
    // 4) Using the dimensions of the image_extents table, and the dimensions of this event's image_extents, 
    //    update the extents table
    // 5) Update the image_extents table with the image_extents vector for this object.
    // 6) Update the image_meta table with the image_meta vector for this object.
    // 7) Update the images table with the images from this event, using the image_extents vector




    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////

    H5::DataSet extents_dataset = group->openDataSet("extents");
    H5::DataSpace extents_dataspace = extents_dataset.getSpace();
    // Get the dataset current size
    hsize_t extents_dims_current[1];
    extents_dataspace.getSimpleExtentDims(extents_dims_current, NULL);


    H5::DataSet image_extents_dataset = group->openDataSet("image_extents");
    H5::DataSpace image_extents_dataspace = image_extents_dataset.getSpace();
    // Get the dataset current size
    hsize_t image_extents_dims_current[1];
    image_extents_dataspace.getSimpleExtentDims(image_extents_dims_current, NULL);

    H5::DataSet image_meta_dataset = group->openDataSet("image_meta");
    H5::DataSpace image_meta_dataspace = image_meta_dataset.getSpace();
        // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    image_meta_dataspace.getSimpleExtentDims(image_meta_dims_current, NULL);


    H5::DataSet images_dataset = group->openDataSet("images");
    H5::DataSpace images_dataspace = images_dataset.getSpace();
    // Get the dataset current size
    hsize_t images_dims_current[1];
    images_dataspace.getSimpleExtentDims(images_dims_current, NULL);

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
    std::vector<ImageMeta2D> image_meta;
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
    extents_dataset.extend(extents_size);

    // Create an extents object to go into the extents table:

    Extents_t next_extents;
    next_extents.first = image_extents_dims_current[0];
    next_extents.n = image_extents.size();

    /////////////////////////////////////////////////////////
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    extents_dataspace = extents_dataset.getSpace();
    extents_dataspace.selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_dims_current);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);


    // Write the new data
    extents_dataset.write(&(next_extents), larcv3::get_datatype<Extents_t>(), extents_memspace, extents_dataspace);



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
    image_extents_dataset.extend(image_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new image_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    image_extents_dataspace = image_extents_dataset.getSpace();
    image_extents_dataspace.selectHyperslab(H5S_SELECT_SET, image_extents_slab_dims, image_extents_dims_current);

    // Define memory space:
    H5::DataSpace image_extents_memspace(1, image_extents_slab_dims);


    // Write the new data
    image_extents_dataset.write(&(image_extents[0]), larcv3::get_datatype<IDExtents_t>(), 
      image_extents_memspace, image_extents_dataspace);


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
    image_meta_dataset.extend(image_meta_size);


    /////////////////////////////////////////////////////////
    // Write the new image_meta to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    image_meta_dataspace = image_meta_dataset.getSpace();
    image_meta_dataspace.selectHyperslab(H5S_SELECT_SET, image_meta_slab_dims, image_meta_dims_current);

    // Define memory space:
    H5::DataSpace image_meta_memspace(1, image_meta_slab_dims);


    // Write the new data
    image_meta_dataset.write(&(image_meta[0]), larcv3::ImageMeta2D::get_datatype(), 
      image_meta_memspace, image_meta_dataspace);




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
    images_dataset.extend(images_size);

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
        images_dataspace = images_dataset.getSpace();
        images_dataspace.selectHyperslab(H5S_SELECT_SET, new_images_slab_dims, offset_images_slab_dims);

        // Define memory space:
        H5::DataSpace images_memspace(1, new_images_slab_dims);


        // Write the new data
        images_dataset.write(&(_image_v.at(image_id).as_vector()[0]), larcv3::get_datatype<float>(), images_memspace, images_dataspace);

        starting_index += new_images_slab_dims[0];
    }







    return;
  }
  void EventImage2D::deserialize(H5::Group * group, size_t entry){
   
    // This function reads in a set of images
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the image_extents table information
    // 3) Use the entry table to get the image_meta information
    // 4) Read the image_meta and use it to build the _image_v vector with correct image sizes
    // 3) Read the images directly into the correct memory locations


    /////////////////////////////////////////////////////////
    // Step 1: Get the extents information from extents dataset
    /////////////////////////////////////////////////////////

    H5::DataSet extents_dataset = group->openDataSet("extents");

    // Get a dataspace inside this file:
    H5::DataSpace extents_dataspace = extents_dataset.getSpace();


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
    extents_dataspace.selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_offset);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);

    Extents_t input_extents;
    // Write the new data
    extents_dataset.read(&(input_extents), larcv3::get_datatype<Extents_t>(), extents_memspace, extents_dataspace);

    /////////////////////////////////////////////////////////
    // Step 2: Get the image_extents information
    /////////////////////////////////////////////////////////


    // Next, open the relevant sections of the data 

    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    H5::DataSet image_extents_dataset = group->openDataSet("image_extents");

    // Get a dataspace inside this file:
    H5::DataSpace image_extents_dataspace = image_extents_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_extents_slab_dims[1];
    image_extents_slab_dims[0] = input_extents.n;

    hsize_t image_extents_offset[1];
    image_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    image_extents_dataspace.selectHyperslab(H5S_SELECT_SET, image_extents_slab_dims, image_extents_offset);


    H5::DataSpace image_extents_memspace(1, image_extents_slab_dims);

    std::vector<IDExtents_t> image_extents;

    // Reserve space for reading in image_extents:
    image_extents.resize(input_extents.n);

    image_extents_dataset.read(&(image_extents[0]), larcv3::get_datatype<IDExtents_t>(), 
      image_extents_memspace, image_extents_dataspace);



    /////////////////////////////////////////////////////////
    // Step 3: Get the image_meta information
    /////////////////////////////////////////////////////////


    H5::DataSet image_meta_dataset = group->openDataSet("image_meta");

    // Get a dataspace inside this file:
    H5::DataSpace image_meta_dataspace = image_meta_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = input_extents.n;

    hsize_t image_meta_offset[1];
    image_meta_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    image_meta_dataspace.selectHyperslab(H5S_SELECT_SET, image_meta_slab_dims, image_meta_offset);


    H5::DataSpace image_meta_memspace(1, image_meta_slab_dims);

    std::vector<ImageMeta2D> image_meta;

    // Reserve space for reading in image_meta:
    image_meta.resize(input_extents.n);

    image_meta_dataset.read(&(image_meta[0]), larcv3::ImageMeta2D::get_datatype(), 
      image_meta_memspace, image_meta_dataspace);

      


    /////////////////////////////////////////////////////////
    // Step 4: Allocate the memory for the images:
    /////////////////////////////////////////////////////////


    _image_v.clear();
    for (size_t image_index = 0; image_index < image_meta.size(); image_index ++){
      _image_v.push_back(Image2D(image_meta.at(image_index)));
    }

    /////////////////////////////////////////////////////////
    // Step 3: Read the images
    /////////////////////////////////////////////////////////


    size_t offset = image_extents.front().first;

    for (size_t image_index = 0; image_index < image_meta.size(); image_index ++){


      // Get the dataset for reading:
      H5::DataSet images_dataset = group->openDataSet("images");

      // Get a dataspace inside this file:
      H5::DataSpace images_dataspace = images_dataset.getSpace();

      // Create a dimension for the data to add (which is the hyperslab data)
      hsize_t images_slab_dims[1];
      images_slab_dims[0] = image_extents.at(image_index).n;

      hsize_t images_offset[1];
      images_offset[0] = offset;

      // std::cout << "N: " << images_slab_dims[0] << "\n"
      //           << "offset: " << images_offset[0] << "\n"
      //           << std::endl;
      // Now, select as a hyperslab the last section of data for readomg:
      images_dataspace.selectHyperslab(H5S_SELECT_SET, images_slab_dims, images_offset);


      H5::DataSpace images_memspace(1, images_slab_dims);


      images_dataset.read(&(_image_v[image_index]._img[0]), larcv3::get_datatype<float>(), images_memspace, images_dataspace);



      offset += images_slab_dims[0];
    }




    return;
  }



}

#endif
