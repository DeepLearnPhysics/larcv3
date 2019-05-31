#ifndef __LARCV3DATAFORMAT_EVENTSPARSETENSOR_CXX
#define __LARCV3DATAFORMAT_EVENTSPARSETENSOR_CXX

#define VOXEL_EXTENTS_CHUNK_SIZE 10
#define VOXEL_IDEXTENTS_CHUNK_SIZE 100
#define VOXEL_META_CHUNK_SIZE 100
#define VOXEL_DATA_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 100
#define VOXEL_COMPRESSION 1

#include "larcv3/core/dataformat/EventSparseTensor.h"


namespace larcv3 {

  /// Global larcv3::EventSparseTensorFactory to register EventSparseTensor
  static EventSparseTensor2DFactory __global_EventSparseTensor2DFactory__;
  static EventSparseTensor3DFactory __global_EventSparseTensor3DFactory__;

  
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
  void EventSparseTensor<dimension>::initialize (H5::Group * group){


    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group->fromClass() << std::endl;
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
    extents_cparms.setDeflate(VOXEL_COMPRESSION);

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
    id_extents_cparms.setDeflate(VOXEL_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet id_extents_ds = group->createDataSet("voxel_extents", 
      id_extents_datatype, id_extents_dataspace, id_extents_cparms);



    /////////////////////////////////////////////////////////
    // Create the meta dataset (ImageMeta<dimension>)
    /////////////////////////////////////////////////////////
    
    // Get the data type for image meta:
    H5::DataType image_meta_datatype = larcv3::ImageMeta<dimension>::get_datatype();

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
    image_meta_cparms.setDeflate(VOXEL_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet image_meta_ds = group->createDataSet("image_meta", 
      image_meta_datatype, image_meta_dataspace, image_meta_cparms);



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
    voxel_cparms.setDeflate(VOXEL_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet voxel_ds = group->createDataSet("voxels", 
      voxel_datatype, voxel_dataspace, voxel_cparms);

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::serialize  (H5::Group * group){

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


    H5::DataSet image_meta_dataset = group->openDataSet("image_meta");
    H5::DataSpace image_meta_dataspace = image_meta_dataset.getSpace();
    // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    image_meta_dataspace.getSimpleExtentDims(image_meta_dims_current, NULL);


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
    // Step 5: Write image meta
    /////////////////////////////////////////////////////////

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t image_meta_slab_dims[1];
    image_meta_slab_dims[0] = image_meta.size();
   

    // Create a size vector for the FULL dataset: previous + current
    hsize_t image_meta_size[1];
    image_meta_size[0] = image_meta_dims_current[0] + image_meta_slab_dims[0];

    // Extend the dataset to accomodate the new data
    image_meta_dataset.extend(image_meta_size);


    // Select as a hyperslab the last section of data for writing:
    image_meta_dataspace = image_meta_dataset.getSpace();
    image_meta_dataspace.selectHyperslab(H5S_SELECT_SET, image_meta_slab_dims, image_meta_dims_current);

    // Define memory space:
    H5::DataSpace image_meta_memspace(1, image_meta_slab_dims);


    // Write the new data
    image_meta_dataset.write(&(image_meta[0]), larcv3::ImageMeta<dimension>::get_datatype(), image_meta_memspace, voxel_extents_dataspace);


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
    voxels_dataset.extend(voxels_size);

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
        voxels_dataspace = voxels_dataset.getSpace();
        voxels_dataspace.selectHyperslab(H5S_SELECT_SET, new_voxels_slab_dims, offset_voxels_slab_dims);

        // Define memory space:
        H5::DataSpace voxels_memspace(1, new_voxels_slab_dims);


        // Write the new data
        voxels_dataset.write(&(_tensor_v.at(projection_id).as_vector()[0]), larcv3::Voxel::get_datatype(), voxels_memspace, voxels_dataspace);

        starting_index += new_voxels_slab_dims[0];
    }




    return;

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::deserialize(H5::Group * group, size_t entry){

    // This function reads in a set of voxels for either sparse tensors or sparse clusters
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the voxel_extents table information
    // 3) Use the entry information to get the image_meta table information
    // 4) Use the voxel_extents information to read the correct voxels
    // 5) Update the meta for each set correctly


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
    // Read this extents entry from the dataset
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

    std::vector<larcv3::ImageMeta<dimension> > image_meta;

    // Reserve space for reading in image_meta:
    image_meta.resize(input_extents.n);

    image_meta_dataset.read(&(image_meta[0]), 
      larcv3::ImageMeta<dimension>::get_datatype(), 
      image_meta_memspace, image_meta_dataspace);

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
