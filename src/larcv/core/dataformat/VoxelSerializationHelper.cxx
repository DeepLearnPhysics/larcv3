#ifndef __LARCV_EVENTVOXEL_SERIALIZATION_HELPER_CXX
#define __LARCV_EVENTVOXEL_SERIALIZATION_HELPER_CXX

#include "VoxelSerializationHelper.h"

#define VOXEL_EXTENTS_CHUNK_SIZE 1
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
  void VoxelSerializationHelper::write_voxels(H5::Group * group, const std::vector<larcv::Voxel> & voxels){
    // TODO

    if (! _initialized){
      _initialized = true;
    }
    return;
  }

  // Read an entry of voxels from a group
  void VoxelSerializationHelper::read_voxels(H5::Group * group, size_t entry, std::vector<larcv::Voxel> & voxels){
    // TODO


    if (! _initialized){
      _initialized = true;
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

  // Function to read all meta from an object
  void VoxelSerializationHelper::read_image_meta(H5::H5Object *obj){

    // Reading a piece of meta is relatively easy.

    //  Read the total number of projections:
      size_t n_projections;
      H5::Attribute n_projections_attr = obj->openAttribute("n_projection_ids");
      n_projections_attr.read(get_datatype<size_t>(),&n_projections);

      // size_t i_meta = 0;
      // for (auto & meta : image_meta){

      //   std::string basename = "proj_" + std::to_string(i_meta);

      //   //  Everything gets flattened to scalars to write meta

      //   // Dimensionality
      //   H5::DataSpace n_dims_space(H5S_SCALAR);
      //   H5::Attribute n_dims_attr = obj->createAttribute(basename + "_n_dims",get_datatype<size_t>(),n_dims_space);
      //   auto n_dims_val = meta.n_dims();
      //   n_dims_attr.write(get_datatype<size_t>(), &(n_dims_val));


      //   // Projection ID
      //   H5::DataSpace projection_id_space(H5S_SCALAR);
      //   H5::Attribute projection_id_attr = obj->createAttribute(basename + "_projection_id",get_datatype<size_t>(),projection_id_space);
      //   auto projection_id_val = meta.projection_id();
      //   projection_id_attr.write(get_datatype<size_t>(), &(projection_id_val));

      //   // Vector characteristics:
      //   for (size_t i_dim = 0; i_dim < n_dims_val; i_dim ++){
      //     // Image Size:
      //     std::string sub_basename = basename + "_" + std::to_string(i_dim);
          
      //     // Image Size
      //     H5::DataSpace image_size_space(H5S_SCALAR);
      //     H5::Attribute image_size_attr = obj->createAttribute(sub_basename + "_image_size",get_datatype<double>(),image_size_space);
      //     auto image_size_val = meta.image_size(i_dim);
      //     image_size_attr.write(get_datatype<double>(), &(image_size_val));

      //     // Number of Voxels
      //     H5::DataSpace number_of_voxels_space(H5S_SCALAR);
      //     H5::Attribute number_of_voxels_attr = obj->createAttribute(sub_basename + "_number_of_voxels",get_datatype<size_t>(),number_of_voxels_space);
      //     auto number_of_voxels_val = meta.number_of_voxels(i_dim);
      //     number_of_voxels_attr.write(get_datatype<size_t>(), &(number_of_voxels_val));

      //     // Origin
      //     H5::DataSpace origin_space(H5S_SCALAR);
      //     H5::Attribute origin_attr = obj->createAttribute(sub_basename + "_origin",get_datatype<double>(),origin_space);
      //     auto origin_val = meta.origin(i_dim);
      //     origin_attr.write(get_datatype<double>(), &(origin_val));


      //   }

      //   i_meta ++;
      // }




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
