#ifndef __LARCV3DATAFORMAT_VOXEL_SERIALIZATION_HELPER_CXX
#define __LARCV3DATAFORMAT_VOXEL_SERIALIZATION_HELPER_CXX

#include "larcv3/core/dataformat/VoxelSerializationHelper.h"

#define VOXEL_EXTENTS_CHUNK_SIZE 100
#define VOXEL_IDEXTENTS_CHUNK_SIZE 100
#define VOXEL_META_CHUNK_SIZE 1000
#define VOXEL_DATA_CHUNK_SIZE 1000

// Here are hard coded values for mapping cluster ID to projection/cluster pairs
#define VOXEL_MAX_PROJECTION_IDS  512
#define VOXEL_MAX_CLUSTER_IDS     2048

namespace larcv3 {


  template<size_t dimension>
  VoxelSerializationHelper<dimension>::VoxelSerializationHelper() :
    _initialized(false)
  {
    // We initialize the image meta to 0, but the default projection meta is hardcoded
    // from the above defines.

    // Upon reading from a file, the default will be overwritten for compatibility

    std::vector<size_t> number_of_voxels;
    std::vector<double> image_sizes;

    number_of_voxels.push_back(VOXEL_MAX_PROJECTION_IDS);
    number_of_voxels.push_back(VOXEL_MAX_CLUSTER_IDS);

    image_sizes.push_back(1.0);
    image_sizes.push_back(1.0);

    projection_meta = ImageMeta2D(0, // size_t projection_id,
                                  number_of_voxels, /// const std::vector<size_t>& number_of_voxels,
                                  image_sizes); ///const std::vector<double>& image_sizes, 


  }

  // Write the projection meta to the group
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::write_projection_meta(H5::Group *group){

    // Take the projection meta, which we define above, and write it:

    if ( ! _initialized ){



        // Get the data type for extents:
        H5::DataType image_meta_datatype = larcv3::ImageMeta2D::get_datatype();

        // Get the starting size (0) and dimensions (unlimited)
        hsize_t image_meta_starting_dim[] = {0};
        hsize_t image_meta_maxsize_dim[]  = {1};

        // Create a dataspace 
        H5::DataSpace image_meta_dataspace(1, image_meta_starting_dim, image_meta_maxsize_dim);

        // Create the extents dataset:
        H5::Attribute image_meta_ds = group->createAttribute("projection_meta", 
          image_meta_datatype, image_meta_dataspace);

        image_meta_ds.write(image_meta_datatype, &projection_meta);

    }

    // TODO
    return;
  }

  // Read the projection meta from the group
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::read_projection_meta(H5::Group *group){
    // TODO
    
    if ( ! _initialized ){

      // Read in the necessary attributes:


        H5::Attribute projection_meta_attr = group->openAttribute("projection_meta");
        H5::DataType projection_meta_type = projection_meta_attr.getDataType();
        projection_meta_attr.read(projection_meta_type,&(projection_meta));
  
      
    }


    return;
  }

  // Write a set of voxels, packaged in the way proscribed by meta, to the group:
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::write_voxels(H5::Group * group, const std::vector<std::vector<larcv3::VoxelSet> > & voxels){
    // TODO

    if (! _initialized){
      LARCV_CRITICAL() << "Must initialize voxel Serialization helper by calling write meta before write voxels." << std::endl;
      throw larbys();
    }

    // Here is the order of operations for voxel serialization:
    // 1) Read the current dimensions of all tables (extents, voxel_extents, voxels)
    // 2) Using the dimensions of the voxel table, build the voxel_extents object for this event
    // 3) Using the dimensions of the voxel_extents table, and the dimensions of this event's voxel_extents, 
    //    update the extents table
    // 4) Update the voxel_extents table with the voxel_extents vector for this object.
    // 5) Update the voxels table with the voxels from this event, using the voxel_extents vector


    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////

    H5::DataSet extents_dataset = group->openDataSet("extents");
    H5::DataSpace extents_dataspace = extents_dataset.getSpace();


    // Get the dataset current size
    hsize_t extents_dims_current[1];
    extents_dataspace.getSimpleExtentDims(extents_dims_current, NULL);


    H5::DataSet voxel_extents_dataset = group->openDataSet("voxel_extents");
    H5::DataSpace voxel_extents_dataspace = voxel_extents_dataset.getSpace();


    // Get the dataset current size
    hsize_t voxel_extents_dims_current[1];
    voxel_extents_dataspace.getSimpleExtentDims(voxel_extents_dims_current, NULL);


    H5::DataSet voxels_dataset = group->openDataSet("voxels");
    H5::DataSpace voxels_dataspace = voxels_dataset.getSpace();


    // Get the dataset current size
    hsize_t voxels_dims_current[1];
    voxels_dataspace.getSimpleExtentDims(voxels_dims_current, NULL);

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

    for (size_t projection_id = 0; projection_id < voxels.size(); projection_id ++){
      for (size_t cluster_id = 0; cluster_id < voxels.at(projection_id).size(); cluster_id ++){
        voxel_extents.resize(voxel_extents.size() + 1);
        voxel_extents.back().n     = voxels.at(projection_id).at(cluster_id).size();
        voxel_extents.back().id    = projection_meta.index(std::vector<size_t>{projection_id, cluster_id});
        voxel_extents.back().first = last_voxel_index;
        last_voxel_index += voxel_extents.back().n;
        n_new_voxels += voxel_extents.back().n;
      }
    }

    // std::cout << "Voxel Extents Size: " << voxel_extents.size() << std::endl;
    // for (auto & e : voxel_extents){
    //   std::cout << "  n: " << e.n << std::endl;
    // }

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
    extents_dataset.extend(extents_size);

    // Create an extents object to go into the extents table:

    Extents_t next_extents;
    next_extents.first = voxel_extents_dims_current[0];
    next_extents.n = voxel_extents.size();

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
    // Step 4: Update the voxel extents table
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t voxel_extents_slab_dims[1];
    voxel_extents_slab_dims[0] = voxel_extents.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t voxel_extents_size[1];
    voxel_extents_size[0] = voxel_extents_dims_current[0] + voxel_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    voxel_extents_dataset.extend(voxel_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new voxel_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    voxel_extents_dataspace = voxel_extents_dataset.getSpace();
    voxel_extents_dataspace.selectHyperslab(H5S_SELECT_SET, voxel_extents_slab_dims, voxel_extents_dims_current);

    // Define memory space:
    H5::DataSpace voxel_extents_memspace(1, voxel_extents_slab_dims);


    // Write the new data
    voxel_extents_dataset.write(&(voxel_extents[0]), larcv3::get_datatype<IDExtents_t>(), voxel_extents_memspace, voxel_extents_dataspace);



    /////////////////////////////////////////////////////////
    // Step 5: Write new voxels
    /////////////////////////////////////////////////////////




    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t voxels_slab_dims[1];
    voxels_slab_dims[0] = n_new_voxels;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t voxels_size[1];
    voxels_size[0] = voxels_dims_current[0] + voxels_slab_dims[0];

    // Extend the dataset to accomodate the new data
    voxels_dataset.extend(voxels_size);

    // Write all of the voxels to file:

    // Keep track of starting index for a write:
    size_t starting_index = voxels_dims_current[0];

    for (size_t projection_id = 0; projection_id < voxels.size(); projection_id ++){
      for (size_t cluster_id = 0; cluster_id < voxels.at(projection_id).size(); cluster_id ++){

        hsize_t new_voxels_slab_dims[1];
        hsize_t offset_voxels_slab_dims[1];

        new_voxels_slab_dims[0] = voxels.at(projection_id).at(cluster_id).size();
        offset_voxels_slab_dims[0] = starting_index;

        // std::cout << "[" << projection_id << "][" << cluster_id << "]: \n"
        //           << "  Offset: " << offset_voxels_slab_dims[0]
        //           << "\n  starting_index: " << starting_index 
        //           << "\n  N: " << new_voxels_slab_dims[0]
        //           << "\n  N: " << voxels.at(projection_id).at(cluster_id).size()
        //           << std::endl;

        // Select as a hyperslab the last section of data for writing:
        voxels_dataspace = voxels_dataset.getSpace();
        voxels_dataspace.selectHyperslab(H5S_SELECT_SET, new_voxels_slab_dims, offset_voxels_slab_dims);

        // Define memory space:
        H5::DataSpace voxels_memspace(1, new_voxels_slab_dims);


        // Write the new data
        voxels_dataset.write(&(voxels.at(projection_id).at(cluster_id).as_vector()[0]), larcv3::Voxel::get_datatype(), voxels_memspace, voxels_dataspace);

        starting_index += new_voxels_slab_dims[0];
      }
    }



    return;
  }

  // Read an entry of voxels from a group
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::read_voxels(H5::Group * group, size_t entry, std::vector<std::vector<larcv3::VoxelSet> > & voxels){
    // TODO


    if (! _initialized){
      LARCV_CRITICAL() << "Must initialize voxel Serialization helper by calling read meta before read voxels." << std::endl;
      throw larbys();
    }

    // This function reads in a set of voxels for either sparse tensors or sparse clusters
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the voxel_extents table information
    // 3) Use the voxel_extents information to read the correct voxels


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
    // Step 2: Get the voxel_extents information
    /////////////////////////////////////////////////////////


    // Next, open the relevant sections of the data 

    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    H5::DataSet voxel_extents_dataset = group->openDataSet("voxel_extents");

    // Get a dataspace inside this file:
    H5::DataSpace voxel_extents_dataspace = voxel_extents_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t voxel_extents_slab_dims[1];
    voxel_extents_slab_dims[0] = input_extents.n;

    hsize_t voxel_extents_offset[1];
    voxel_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    voxel_extents_dataspace.selectHyperslab(H5S_SELECT_SET, voxel_extents_slab_dims, voxel_extents_offset);


    H5::DataSpace voxel_extents_memspace(1, voxel_extents_slab_dims);

    std::vector<IDExtents_t> voxel_extents;

    // Reserve space for reading in voxel_extents:
    voxel_extents.resize(input_extents.n);

    voxel_extents_dataset.read(&(voxel_extents[0]), larcv3::get_datatype<IDExtents_t>(), voxel_extents_memspace, voxel_extents_dataspace);

    // std::cout << "voxel_extents.size(): " << voxel_extents.size() << std::endl;
    

    /////////////////////////////////////////////////////////
    // Step 3: Read the voxels
    /////////////////////////////////////////////////////////

    voxels.clear();
    voxels.resize(image_meta.size());

    size_t offset = voxel_extents.front().first;

    for (size_t voxel_set_index = 0; voxel_set_index < voxel_extents.size(); voxel_set_index ++){

      // First, figure out the projection/cluster ID of this voxel set:
      auto coords = projection_meta.coordinates(voxel_extents.at(voxel_set_index).id);

      size_t projection_id = coords[0];
      size_t cluster_id = coords[1];

      // Make sure there is room for this:
      if(voxels.at(projection_id).size() <= cluster_id){
        voxels.at(projection_id).resize(cluster_id + 1);
      }

      // Get the dataset for reading:
      H5::DataSet voxels_dataset = group->openDataSet("voxels");

      // Get a dataspace inside this file:
      H5::DataSpace voxels_dataspace = voxels_dataset.getSpace();

      // Create a dimension for the data to add (which is the hyperslab data)
      hsize_t voxels_slab_dims[1];
      voxels_slab_dims[0] = voxel_extents.at(voxel_set_index).n;

      hsize_t voxels_offset[1];
      voxels_offset[0] = offset;

      // std::cout << "N: " << voxels_slab_dims[0] << "\n"
      //           << "offset: " << voxels_offset[0] << "\n"
      //           << std::endl;
      // Now, select as a hyperslab the last section of data for readomg:
      voxels_dataspace.selectHyperslab(H5S_SELECT_SET, voxels_slab_dims, voxels_offset);


      H5::DataSpace voxels_memspace(1, voxels_slab_dims);

      //// TODO
      // This implementation is not ideal.
      // It copies from disk, then is copying into a vector

      std::vector<larcv3::Voxel> temp_voxel_vector;
      temp_voxel_vector.resize(voxels_slab_dims[0]);

      // Reserve space for reading in voxels:

      voxels_dataset.read(&(temp_voxel_vector[0]), larcv3::Voxel::get_datatype(), voxels_memspace, voxels_dataspace);

      // std::cout << "temp_voxel_vector.size(): " << temp_voxel_vector.size() << std::endl;


      for (auto & v : temp_voxel_vector){
        voxels.at(projection_id).at(cluster_id).add(v);
      }
      voxels.at(projection_id).at(cluster_id).id(cluster_id);
      // std::cout << "voxels.at(projection_id).at(cluster_id).size(): " << voxels.at(projection_id).at(cluster_id).size() << std::endl;
    
      offset += voxels_slab_dims[0];
    }



    return;
  }


  // Function to write a piece of meta as an attribute
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::write_image_meta(H5::H5Object * obj){

    // Writing a piece of meta is relatively easy.
    //  Write the total number of projections:

    H5::DataSpace n_projections_space(H5S_SCALAR);
    H5::Attribute projection_attr = obj->createAttribute("n_projection_ids",get_datatype<size_t>(),n_projections_space);
    size_t n_projections = image_meta.size();
    projection_attr.write(get_datatype<size_t>(), &n_projections);

    size_t i_meta = 0;
    for (auto & meta : image_meta){

        std::string basename = "proj_" + std::to_string(i_meta);

       // Get the data type for extents:
        H5::DataType image_meta_datatype = meta.get_datatype();

        // Get the starting size (0) and dimensions (unlimited)
        hsize_t image_meta_starting_dim[] = {1};
        hsize_t image_meta_maxsize_dim[]  = {1};

        // Create a dataspace 
        H5::DataSpace image_meta_dataspace(1, image_meta_starting_dim, image_meta_maxsize_dim);

        // Create the extents dataset:
        H5::Attribute image_meta_ds = obj->createAttribute(basename + "_meta", 
          image_meta_datatype, image_meta_dataspace);

        image_meta_ds.write(image_meta_datatype, &meta);


      i_meta ++;
    }

  }

  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::initialize_for_write(H5::Group * obj){
    write_projection_meta(obj);
    write_image_meta(obj);
    _initialized = true;
  }

  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::initialize_for_read(H5::Group * obj){
    read_projection_meta(obj);
    image_meta.clear();
    read_image_meta(obj);
    _initialized = true;
  }

  // Function to read all meta from an object
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::read_image_meta(H5::H5Object *obj){

    // Reading a piece of meta is relatively easy.

    //  Read the total number of projections:
    size_t n_projections;
    H5::Attribute n_projections_attr = obj->openAttribute("n_projection_ids");
    n_projections_attr.read(get_datatype<size_t>(),&n_projections);
    
    // Initialize empty meta
    image_meta.clear();
    image_meta.resize(n_projections);




    // size_t i_meta = 0;
    for (size_t i_meta = 0; i_meta < n_projections;i_meta ++){

        std::string basename = "proj_" + std::to_string(i_meta);

        H5::Attribute image_meta_attr = obj->openAttribute(basename + "_meta");
        H5::DataType image_meta_type = image_meta_attr.getDataType();
        image_meta_attr.read(image_meta_type,&(image_meta[i_meta]));
  
      // //  Everything gets flattened to scalars to write meta

      // // Dimensionality
      // size_t n_dims;
      // H5::Attribute n_dims_attr = obj->openAttribute(basename + "_n_dims");
      // n_dims_attr.read(get_datatype<size_t>(), &(n_dims));


      // // Projection ID
      // size_t projection_id;
      // H5::Attribute projection_id_attr = obj->openAttribute(basename + "_projection_id");
      // projection_id_attr.read(get_datatype<size_t>(), &(projection_id));

      // image_meta.at(i_meta).set_projection_id(projection_id);

      // // Vector characteristics:
      // for (size_t i_dim = 0; i_dim < n_dims; i_dim ++){
      //   // Image Size:
      //   std::string sub_basename = basename + "_" + std::to_string(i_dim);
        
      //   // Image Size
      //   double image_size_val;
      //   H5::Attribute image_size_attr = obj->openAttribute(sub_basename + "_image_size");
      //   image_size_attr.read(get_datatype<double>(), &(image_size_val));

      //   // Number of Voxels
      //   size_t number_of_voxels_val;
      //   H5::Attribute number_of_voxels_attr = obj->openAttribute(sub_basename + "_number_of_voxels");
      //   number_of_voxels_attr.read(get_datatype<size_t>(), &(number_of_voxels_val));

      //   // Origin
      //   double origin_val;
      //   H5::Attribute origin_attr = obj->openAttribute(sub_basename + "_origin");
      //   origin_attr.read(get_datatype<double>(), &(origin_val));

      //   image_meta.at(i_meta).add_dimension(image_size_val, number_of_voxels_val, origin_val);


      // }

    }

    return;
  }


  // Initialization is the same for sparse image and cluster set
  template<size_t dimension>
  void VoxelSerializationHelper<dimension>::initialize_voxel_group(H5::Group * group){

    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group->fromClass() << std::endl;
      throw larbys();
    }

    // Initialization creates three tables for a set of voxels:
    // Extents (Traditional extents, but maps to the next table)
    // VoxelExtents (Extents but with an ID for each entry)
    // Voxels (A big table of voxels.)


    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // IDExtents in the data tree.


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
    hsize_t      extents_chunk_dims[1] ={VOXEL_EXTENTS_CHUNK_SIZE};
    extents_cparms.setChunk( 1, extents_chunk_dims );

    // Create the extents dataset:
    H5::DataSet extents_ds = group->createDataSet("extents", extents_datatype, extents_dataspace, extents_cparms);



    /////////////////////////////////////////////////////////
    // Create the ID extents dataset (VoxelExtents)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    H5::DataType id_extents_datatype = larcv3::get_datatype<IDExtents_t>();


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t id_extents_starting_dim[] = {0};
    hsize_t id_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace id_extents_dataspace(1, id_extents_starting_dim, id_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList id_extents_cparms;
    hsize_t      id_extents_chunk_dims[1] ={VOXEL_IDEXTENTS_CHUNK_SIZE};
    id_extents_cparms.setChunk( 1, id_extents_chunk_dims );

    // Create the extents dataset:
    H5::DataSet id_extents_ds = group->createDataSet("voxel_extents", 
      id_extents_datatype, id_extents_dataspace, id_extents_cparms);



    /////////////////////////////////////////////////////////
    // Create the voxels dataset (Voxels)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    H5::DataType voxel_datatype = larcv3::Voxel::get_datatype();


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

    // Create the extents dataset:
    H5::DataSet voxel_ds = group->createDataSet("voxels", 
      voxel_datatype, voxel_dataspace, voxel_cparms);
  }

// Force instantiation of 2D and 3D serialization helpers:
template class VoxelSerializationHelper<2>;
template class VoxelSerializationHelper<3>;
  
}

#endif
