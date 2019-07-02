#ifndef __LARCV3DATAFORMAT_EVENTSPARSETENSOR_CXX
#define __LARCV3DATAFORMAT_EVENTSPARSETENSOR_CXX

#define VOXEL_EXTENTS_CHUNK_SIZE 10
#define VOXEL_IDEXTENTS_CHUNK_SIZE 100
#define VOXEL_META_CHUNK_SIZE 100
#define VOXEL_DATA_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 100
#define VOXEL_COMPRESSION 1

#define EXTENTS_DATASET 0
#define VOXEL_EXTENTS_DATASET 1
#define IMAGE_META_DATASET 2
#define VOXELS_DATASET 3
#define N_DATASETS 4

#include "larcv3/core/dataformat/EventSparseTensor.h"


namespace larcv3 {

  /// Global larcv3::EventSparseTensorFactory to register EventSparseTensor
  static EventSparseTensor2DFactory __global_EventSparseTensor2DFactory__;
  static EventSparseTensor3DFactory __global_EventSparseTensor3DFactory__;

  
  template<size_t dimension> 
  EventSparseTensor<dimension>::EventSparseTensor(){

    _data_types.resize(N_DATASETS);

    _data_types[EXTENTS_DATASET]       = new H5::DataType(larcv3::get_datatype<Extents_t>());
    _data_types[VOXEL_EXTENTS_DATASET] = new H5::DataType(larcv3::get_datatype<IDExtents_t>());
    _data_types[IMAGE_META_DATASET]    = new H5::DataType(larcv3::ImageMeta<dimension>::get_datatype());
    _data_types[VOXELS_DATASET]        = new H5::DataType(larcv3::Voxel::get_datatype());

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
    // H5::DataType extents_datatype = larcv3::get_datatype<Extents_t>();


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
    H5::DataSet extents_ds = group->createDataSet("extents", 
      *_data_types[EXTENTS_DATASET], extents_dataspace, extents_cparms);



    /////////////////////////////////////////////////////////
    // Create the ID extents dataset (VoxelExtents)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    // H5::DataType id_extents_datatype = larcv3::get_datatype<IDExtents_t>();


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
      *_data_types[VOXEL_EXTENTS_DATASET], id_extents_dataspace, id_extents_cparms);



    /////////////////////////////////////////////////////////
    // Create the meta dataset (ImageMeta<dimension>)
    /////////////////////////////////////////////////////////
    
    // Get the data type for image meta:
    // H5::DataType image_meta_datatype = larcv3::ImageMeta<dimension>::get_datatype();

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
      *_data_types[IMAGE_META_DATASET], image_meta_dataspace, image_meta_cparms);



    /////////////////////////////////////////////////////////
    // Create the voxels dataset (Voxels)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    // H5::DataType voxel_datatype = larcv3::Voxel::get_datatype();


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
      *_data_types[VOXELS_DATASET], voxel_dataspace, voxel_cparms);

  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::open_in_datasets(H5::Group * group){

    if (_open_in_datasets.size() < N_DATASETS ){
       _open_in_datasets.resize(N_DATASETS);
       _open_in_dataspaces.resize(N_DATASETS);
       
       _open_in_datasets[EXTENTS_DATASET]         = group->openDataSet("extents");
       _open_in_dataspaces[EXTENTS_DATASET]       = _open_in_datasets[EXTENTS_DATASET].getSpace();

       _open_in_datasets[VOXEL_EXTENTS_DATASET]   = group->openDataSet("voxel_extents");
       _open_in_dataspaces[VOXEL_EXTENTS_DATASET] = _open_in_datasets[VOXEL_EXTENTS_DATASET].getSpace();

       _open_in_datasets[IMAGE_META_DATASET]      = group->openDataSet("image_meta");
       _open_in_dataspaces[IMAGE_META_DATASET]    = _open_in_datasets[IMAGE_META_DATASET].getSpace();

       _open_in_datasets[VOXELS_DATASET]          = group->openDataSet("voxels");
       _open_in_dataspaces[VOXELS_DATASET]        = _open_in_datasets[VOXELS_DATASET].getSpace();
;
     }

    return;
  }

  template<size_t dimension> 
  void EventSparseTensor<dimension>::open_out_datasets(H5::Group * group){

    if (_open_out_datasets.size() < N_DATASETS ){
       _open_out_datasets.resize(N_DATASETS);
       _open_out_dataspaces.resize(N_DATASETS);
       
       _open_out_datasets[EXTENTS_DATASET]         = group->openDataSet("extents");
       _open_out_dataspaces[EXTENTS_DATASET]       = _open_out_datasets[EXTENTS_DATASET].getSpace();

       _open_out_datasets[VOXEL_EXTENTS_DATASET]   = group->openDataSet("voxel_extents");
       _open_out_dataspaces[VOXEL_EXTENTS_DATASET] = _open_out_datasets[VOXEL_EXTENTS_DATASET].getSpace();

       _open_out_datasets[IMAGE_META_DATASET]      = group->openDataSet("image_meta");
       _open_out_dataspaces[IMAGE_META_DATASET]    = _open_out_datasets[IMAGE_META_DATASET].getSpace();

       _open_out_datasets[VOXELS_DATASET]          = group->openDataSet("voxels");
       _open_out_dataspaces[VOXELS_DATASET]        = _open_out_datasets[VOXELS_DATASET].getSpace();
    }

    return;
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


    open_out_datasets(group);

    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////

    // H5::DataSet _open_datasets[EXTENTS_DATASET] = group->openDataSet("extents");
    // H5::DataSpace extents_dataspace = _open_datasets[EXTENTS_DATASET].getSpace();
    // Get the dataset current size
    hsize_t extents_dims_current[1];
    _open_out_dataspaces[EXTENTS_DATASET].getSimpleExtentDims(extents_dims_current, NULL);


    // H5::DataSet voxel_extents_dataset = group->openDataSet("voxel_extents");
    // H5::DataSpace voxel_extents_dataspace = _open_out_datasets[VOXEL_EXTENTS_DATASET].getSpace();
    // Get the dataset current size
    hsize_t voxel_extents_dims_current[1];
    _open_out_dataspaces[VOXEL_EXTENTS_DATASET].getSimpleExtentDims(voxel_extents_dims_current, NULL);


    // H5::DataSet image_meta_dataset = group->openDataSet("image_meta");
    // H5::DataSpace image_meta_dataspace = image_meta_dataset.getSpace();
    // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    _open_out_dataspaces[IMAGE_META_DATASET].getSimpleExtentDims(image_meta_dims_current, NULL);


    // H5::DataSet voxels_dataset = group->openDataSet("voxels");
    // H5::DataSpace voxels_dataspace = voxels_dataset.getSpace();
    // Get the dataset current size
    hsize_t voxels_dims_current[1];
    _open_out_dataspaces[VOXELS_DATASET].getSimpleExtentDims(voxels_dims_current, NULL);

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
    _open_out_datasets[EXTENTS_DATASET].extend(extents_size);

    // Create an extents object to go into the extents table:

    Extents_t next_extents;
    next_extents.first = voxel_extents_dims_current[0];
    next_extents.n = voxel_extents.size();

    /////////////////////////////////////////////////////////
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[EXTENTS_DATASET] = _open_out_datasets[EXTENTS_DATASET].getSpace();
    _open_out_dataspaces[EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_dims_current);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);


    // Write the new data
    _open_out_datasets[EXTENTS_DATASET].write(&(next_extents), 
      *_data_types[EXTENTS_DATASET], 
      extents_memspace,
      _open_out_dataspaces[EXTENTS_DATASET]);



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
    _open_out_datasets[VOXEL_EXTENTS_DATASET].extend(voxel_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new voxel_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[VOXEL_EXTENTS_DATASET] = _open_out_datasets[VOXEL_EXTENTS_DATASET].getSpace();
    _open_out_dataspaces[VOXEL_EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET, 
      voxel_extents_slab_dims, 
      voxel_extents_dims_current);

    // Define memory space:
    H5::DataSpace voxel_extents_memspace(1, voxel_extents_slab_dims);


    // Write the new data
    _open_out_datasets[VOXEL_EXTENTS_DATASET].write(&(voxel_extents[0]),
      *_data_types[VOXEL_EXTENTS_DATASET], 
      voxel_extents_memspace, 
      _open_out_dataspaces[VOXEL_EXTENTS_DATASET]);

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
    _open_out_datasets[IMAGE_META_DATASET].extend(image_meta_size);


    // Select as a hyperslab the last section of data for writing:
    _open_out_dataspaces[IMAGE_META_DATASET] = _open_out_datasets[IMAGE_META_DATASET].getSpace();
    _open_out_dataspaces[IMAGE_META_DATASET].selectHyperslab(H5S_SELECT_SET, image_meta_slab_dims, image_meta_dims_current);

    // Define memory space:
    H5::DataSpace image_meta_memspace(1, image_meta_slab_dims);


    // Write the new data
    _open_out_datasets[IMAGE_META_DATASET].write(&(image_meta[0]), 
      *_data_types[IMAGE_META_DATASET], 
      image_meta_memspace, 
      _open_out_dataspaces[IMAGE_META_DATASET]);


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
    _open_out_datasets[VOXELS_DATASET].extend(voxels_size);
    _open_out_dataspaces[VOXELS_DATASET] = _open_out_datasets[VOXELS_DATASET].getSpace();

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
        _open_out_dataspaces[VOXELS_DATASET].selectHyperslab(H5S_SELECT_SET, new_voxels_slab_dims, offset_voxels_slab_dims);

        // Define memory space:
        H5::DataSpace voxels_memspace(1, new_voxels_slab_dims);


        // Write the new data
        _open_out_datasets[VOXELS_DATASET].write(&(_tensor_v.at(projection_id).as_vector()[0]), 
          *_data_types[VOXELS_DATASET], 
          voxels_memspace, 
          _open_out_dataspaces[VOXELS_DATASET]);

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
    _open_in_dataspaces[EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_offset);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);

    Extents_t input_extents;
    // Write the new data
    _open_in_datasets[EXTENTS_DATASET].read(&(input_extents), 
      *_data_types[EXTENTS_DATASET],
      extents_memspace, 
      _open_in_dataspaces[EXTENTS_DATASET]);

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
    _open_in_dataspaces[VOXEL_EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET,
      voxel_extents_slab_dims, voxel_extents_offset);


    H5::DataSpace voxel_extents_memspace(1, voxel_extents_slab_dims);

    std::vector<IDExtents_t> voxel_extents;

    // Reserve space for reading in voxel_extents:
    voxel_extents.resize(input_extents.n);

    _open_in_datasets[VOXEL_EXTENTS_DATASET].read(&(voxel_extents[0]), 
      *_data_types[VOXEL_EXTENTS_DATASET], 
      voxel_extents_memspace, 
      _open_in_dataspaces[VOXEL_EXTENTS_DATASET]);

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
    _open_in_dataspaces[IMAGE_META_DATASET].selectHyperslab(H5S_SELECT_SET, 
      image_meta_slab_dims, image_meta_offset);


    H5::DataSpace image_meta_memspace(1, image_meta_slab_dims);

    std::vector<larcv3::ImageMeta<dimension> > image_meta;

    // Reserve space for reading in image_meta:
    image_meta.resize(input_extents.n);

    _open_in_datasets[IMAGE_META_DATASET].read(&(image_meta[0]), 
      *_data_types[IMAGE_META_DATASET], 
      image_meta_memspace, _open_in_dataspaces[IMAGE_META_DATASET]);

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
      _open_in_dataspaces[VOXELS_DATASET].selectHyperslab(H5S_SELECT_SET, voxels_slab_dims, voxels_offset);


      H5::DataSpace voxels_memspace(1, voxels_slab_dims);

      //// TODO
      // This implementation is not ideal.
      // It copies from disk, then is copying into a vector

      std::vector<larcv3::Voxel> temp_voxel_vector;
      temp_voxel_vector.resize(voxels_slab_dims[0]);

      // Reserve space for reading in voxels:

      _open_in_datasets[VOXELS_DATASET].read(&(temp_voxel_vector[0]), *_data_types[VOXELS_DATASET],
       voxels_memspace, _open_in_dataspaces[VOXELS_DATASET]);

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
