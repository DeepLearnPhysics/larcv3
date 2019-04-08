#ifndef __LARCV_EVENTVOXEL_SERIALIZATION_HELPER_CXX
#define __LARCV_EVENTVOXEL_SERIALIZATION_HELPER_CXX

#include "VoxelSerializationHelper.h"

#define VOXEL_EXTENTS_CHUNK_SIZE 10
#define VOXEL_IDEXTENTS_CHUNK_SIZE 10
#define VOXEL_DATA_CHUNK_SIZE 1000

// Here are hard coded values for mapping cluster ID to projection/cluster pairs
#define VOXEL_MAX_PROJECTION_IDS  512
#define VOXEL_MAX_CLUSTER_IDS     2048

namespace larcv {


  VoxelSerializationHelper::VoxelSerializationHelper() :
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

    projection_meta = ImageMeta(2, // size_t n_dims, 
                                0, // size_t projection_id,
                                number_of_voxels, /// const std::vector<size_t>& number_of_voxels,
                                image_sizes); ///const std::vector<double>& image_sizes, 


  }

  // Write the projection meta to the group
  void VoxelSerializationHelper::write_projection_meta(H5::Group *group){

    // Take the projection meta, which we define above, and write it:

    if ( ! _initialized ){
      //  To rebuild the projection meta, we need only a few attibutes: n_voxels_0, n_voxels_1

      H5::DataSpace dspace0(H5S_SCALAR);
      H5::Attribute att0 = group->createAttribute("max_projection_ids",get_datatype<size_t>(),dspace0);
      size_t n_projections = projection_meta.number_of_voxels(0);
      att0.write(get_datatype<size_t>(), &n_projections);

      H5::DataSpace dspace1(H5S_SCALAR);
      H5::Attribute att1 = group->createAttribute("max_cluster_ids",get_datatype<size_t>(),dspace1);
      size_t n_clusters = projection_meta.number_of_voxels(1);
      att1.write(get_datatype<size_t>(), &n_clusters);

    }

    // TODO
    return;
  }

  // Read the projection meta from the group
  void VoxelSerializationHelper::read_projection_meta(H5::Group *group){
    // TODO
    
    if ( ! _initialized ){

      // Read in the necessary attributes:



      std::vector<size_t> number_of_voxels;
      std::vector<double> image_sizes;

      number_of_voxels.resize(2);

      image_sizes.push_back(1.0);
      image_sizes.push_back(1.0);

      H5::Attribute att0 = group->openAttribute("max_projection_ids");
      H5::DataType type0 = att0.getDataType();
      att0.read(type0,&(number_of_voxels[0]));

      H5::Attribute att1 = group->openAttribute("max_cluster_ids");
      H5::DataType type1 = att1.getDataType();
      att1.read(type1,&(number_of_voxels[1]));


      projection_meta = ImageMeta(2, // size_t n_dims, 
                                  0, // size_t projection_id,
                                  number_of_voxels, /// const std::vector<size_t>& number_of_voxels,
                                  image_sizes); ///const std::vector<double>& image_sizes, 

      
    }


    return;
  }

  // Write a set of voxels, packaged in the way proscribed by meta, to the group:
  void VoxelSerializationHelper::write_voxels(H5::Group * group, const std::vector<std::vector<larcv::VoxelSet> > & voxels){
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
    extents_dataset.write(&(next_extents), larcv::get_datatype<Extents_t>(), extents_memspace, extents_dataspace);



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
    voxel_extents_dataset.write(&(voxel_extents[0]), larcv::get_datatype<IDExtents_t>(), voxel_extents_memspace, voxel_extents_dataspace);



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
        voxels_dataset.write(&(voxels.at(projection_id).at(cluster_id).as_vector()[0]), larcv::Voxel::get_datatype(), voxels_memspace, voxels_dataspace);

        starting_index += new_voxels_slab_dims[0];
      }
    }



    return;
  }

  // Read an entry of voxels from a group
  void VoxelSerializationHelper::read_voxels(H5::Group * group, size_t entry, std::vector<std::vector<larcv::VoxelSet> > & voxels){
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
    extents_dataset.read(&(input_extents), larcv::get_datatype<Extents_t>(), extents_memspace, extents_dataspace);

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

    voxel_extents_dataset.read(&(voxel_extents[0]), larcv::get_datatype<IDExtents_t>(), voxel_extents_memspace, voxel_extents_dataspace);

    std::cout << "voxel_extents.size(): " << voxel_extents.size() << std::endl;
    

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

      std::cout << "N: " << voxels_slab_dims[0] << "\n"
                << "offset: " << voxels_offset[0] << "\n"
                << std::endl;
      // Now, select as a hyperslab the last section of data for readomg:
      voxels_dataspace.selectHyperslab(H5S_SELECT_SET, voxels_slab_dims, voxels_offset);


      H5::DataSpace voxels_memspace(1, voxels_slab_dims);

      //// TODO
      // This implementation is not ideal.
      // It copies from disk, then is copying into a vector

      std::vector<larcv::Voxel> temp_voxel_vector;
      temp_voxel_vector.resize(voxels_slab_dims[0]);

      // Reserve space for reading in voxels:

      voxels_dataset.read(&(temp_voxel_vector[0]), larcv::Voxel::get_datatype(), voxels_memspace, voxels_dataspace);

      std::cout << "temp_voxel_vector.size(): " << temp_voxel_vector.size() << std::endl;

      std::cout << "voxels.size(): " << voxels.size() << std::endl;
      std::cout << "cluster id: " << cluster_id << std::endl;
      std::cout << "voxels.at(" << projection_id  << ").size(): " << voxels.at(projection_id).size() << std::endl;

      for (auto & v : temp_voxel_vector){
        voxels.at(projection_id).at(cluster_id).add(v);
      }

      std::cout << "voxels.at(projection_id).at(cluster_id).size(): " << voxels.at(projection_id).at(cluster_id).size() << std::endl;
    

    }



    return;
  }


  // Function to write a piece of meta as an attribute
  void VoxelSerializationHelper::write_image_meta(H5::H5Object * obj){

    // Writing a piece of meta is relatively easy.
    // Instead of writing a complex object, we write it as just a set of scalars.

    //  Write the total number of projections:

    H5::DataSpace n_projections_space(H5S_SCALAR);
    H5::Attribute projection_attr = obj->createAttribute("n_projection_ids",get_datatype<size_t>(),n_projections_space);
    size_t n_projections = image_meta.size();
    projection_attr.write(get_datatype<size_t>(), &n_projections);

    size_t i_meta = 0;
    for (auto & meta : image_meta){

      std::string basename = "proj_" + std::to_string(i_meta);

      //  Everything gets flattened to scalars to write meta

      // Dimensionality
      H5::DataSpace n_dims_space(H5S_SCALAR);
      H5::Attribute n_dims_attr = obj->createAttribute(basename + "_n_dims",get_datatype<size_t>(),n_dims_space);
      auto n_dims_val = meta.n_dims();
      n_dims_attr.write(get_datatype<size_t>(), &(n_dims_val));


      // Projection ID
      H5::DataSpace projection_id_space(H5S_SCALAR);
      H5::Attribute projection_id_attr = obj->createAttribute(basename + "_projection_id",get_datatype<size_t>(),projection_id_space);
      auto projection_id_val = meta.projection_id();
      projection_id_attr.write(get_datatype<size_t>(), &(projection_id_val));

      // Vector characteristics:
      for (size_t i_dim = 0; i_dim < n_dims_val; i_dim ++){
        // Image Size:
        std::string sub_basename = basename + "_" + std::to_string(i_dim);
        
        // Image Size
        H5::DataSpace image_size_space(H5S_SCALAR);
        H5::Attribute image_size_attr = obj->createAttribute(sub_basename + "_image_size",get_datatype<double>(),image_size_space);
        auto image_size_val = meta.image_size(i_dim);
        image_size_attr.write(get_datatype<double>(), &(image_size_val));

        // Number of Voxels
        H5::DataSpace number_of_voxels_space(H5S_SCALAR);
        H5::Attribute number_of_voxels_attr = obj->createAttribute(sub_basename + "_number_of_voxels",get_datatype<size_t>(),number_of_voxels_space);
        auto number_of_voxels_val = meta.number_of_voxels(i_dim);
        number_of_voxels_attr.write(get_datatype<size_t>(), &(number_of_voxels_val));

        // Origin
        H5::DataSpace origin_space(H5S_SCALAR);
        H5::Attribute origin_attr = obj->createAttribute(sub_basename + "_origin",get_datatype<double>(),origin_space);
        auto origin_val = meta.origin(i_dim);
        origin_attr.write(get_datatype<double>(), &(origin_val));


      }

      i_meta ++;
    }



     

  }
  void VoxelSerializationHelper::initialize_for_write(H5::Group * obj){
    write_projection_meta(obj);
    write_image_meta(obj);
    _initialized = true;
  }

  void VoxelSerializationHelper::initialize_for_read(H5::Group * obj){
    read_projection_meta(obj);
    image_meta.clear();
    read_image_meta(obj);
    _initialized = true;
  }

  // Function to read all meta from an object
  void VoxelSerializationHelper::read_image_meta(H5::H5Object *obj){

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

      //  Everything gets flattened to scalars to write meta

      // Dimensionality
      size_t n_dims;
      H5::Attribute n_dims_attr = obj->openAttribute(basename + "_n_dims");
      n_dims_attr.read(get_datatype<size_t>(), &(n_dims));


      // Projection ID
      size_t projection_id;
      H5::Attribute projection_id_attr = obj->openAttribute(basename + "_projection_id");
      projection_id_attr.read(get_datatype<size_t>(), &(projection_id));

      image_meta.at(i_meta).set_projection_id(projection_id);

      // Vector characteristics:
      for (size_t i_dim = 0; i_dim < n_dims; i_dim ++){
        // Image Size:
        std::string sub_basename = basename + "_" + std::to_string(i_dim);
        
        // Image Size
        double image_size_val;
        H5::Attribute image_size_attr = obj->openAttribute(sub_basename + "_image_size");
        image_size_attr.read(get_datatype<double>(), &(image_size_val));

        // Number of Voxels
        size_t number_of_voxels_val;
        H5::Attribute number_of_voxels_attr = obj->openAttribute(sub_basename + "_number_of_voxels");
        number_of_voxels_attr.read(get_datatype<size_t>(), &(number_of_voxels_val));

        // Origin
        double origin_val;
        H5::Attribute origin_attr = obj->openAttribute(sub_basename + "_origin");
        origin_attr.read(get_datatype<double>(), &(origin_val));

        image_meta.at(i_meta).add_dimension(image_size_val, number_of_voxels_val, origin_val);


      }

    }

    return;
  }


  // Initialization is the same for sparse image and cluster set
  void VoxelSerializationHelper::initialize_voxel_group(H5::Group * group){

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
    H5::DataType extents_datatype = larcv::get_datatype<Extents_t>();


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
    H5::DataType id_extents_datatype = larcv::get_datatype<IDExtents_t>();


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

    // Create the extents dataset:
    H5::DataSet voxel_ds = group->createDataSet("voxels", 
      voxel_datatype, voxel_dataspace, voxel_cparms);
  }


  
}

#endif
