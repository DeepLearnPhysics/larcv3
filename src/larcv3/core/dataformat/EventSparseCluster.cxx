#ifndef __LARCV3DATAFORMAT_EVENTSPARSECLUSTER_CXX
#define __LARCV3DATAFORMAT_EVENTSPARSECLUSTER_CXX

#include "larcv3/core/dataformat/EventSparseCluster.h"

#define VOXEL_EXTENTS_CHUNK_SIZE 10
#define VOXEL_IDEXTENTS_CHUNK_SIZE 100
#define VOXEL_META_CHUNK_SIZE 100
#define VOXEL_DATA_CHUNK_SIZE 1000
#define IMAGE_META_CHUNK_SIZE 100
#define VOXEL_COMPRESSION 1

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
  void EventSparseCluster<dimension>::initialize (H5::Group * group){


    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group->fromClass() << std::endl;
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
    // Create the ID extents dataset (ProjectionExtents)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    H5::DataType projection_extents_datatype = larcv3::get_datatype<IDExtents_t>();


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t projection_extents_starting_dim[] = {0};
    hsize_t projection_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace projection_extents_dataspace(1, projection_extents_starting_dim, projection_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList projection_extents_cparms;
    hsize_t      projection_extents_chunk_dims[1] ={VOXEL_IDEXTENTS_CHUNK_SIZE};
    projection_extents_cparms.setChunk( 1, projection_extents_chunk_dims );
    projection_extents_cparms.setDeflate(VOXEL_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet projection_extents_ds = group->createDataSet("projection_extents", 
      projection_extents_datatype, projection_extents_dataspace, projection_extents_cparms);



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
    // Create the Cluster extents dataset (IDExtents)
    /////////////////////////////////////////////////////////

    // Get the data type for extents:
    H5::DataType cluster_extents_datatype = larcv3::get_datatype<IDExtents_t>();


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t cluster_extents_starting_dim[] = {0};
    hsize_t cluster_extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace cluster_extents_dataspace(1, cluster_extents_starting_dim, cluster_extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList cluster_extents_cparms;
    hsize_t      cluster_extents_chunk_dims[1] ={VOXEL_IDEXTENTS_CHUNK_SIZE};
    cluster_extents_cparms.setChunk( 1, cluster_extents_chunk_dims );
    cluster_extents_cparms.setDeflate(VOXEL_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet cluster_extents_ds = group->createDataSet("cluster_extents", 
      cluster_extents_datatype, cluster_extents_dataspace, cluster_extents_cparms);


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
  void EventSparseCluster<dimension>::open_datasets(H5::Group * group){

    if (_open_datasets.size() < N_DATASETS ){
        std::cout << "Opening datasets" << std::endl;
       _open_datasets.resize(N_DATASETS);
       _open_dataspaces.resize(N_DATASETS);
       
       _open_datasets[EXTENTS_DATASET]         = group->openDataSet("extents");
       _open_dataspaces[EXTENTS_DATASET]       = _open_datasets[EXTENTS_DATASET].getSpace();

       _open_datasets[PROJECTION_DATASET]         = group->openDataSet("projection");
       _open_dataspaces[PROJECTION_DATASET]       = _open_datasets[PROJECTION_DATASET].getSpace();


       _open_datasets[CLUSTER_EXTENTS_DATASET]   = group->openDataSet("cluster_extents");
       _open_dataspaces[CLUSTER_EXTENTS_DATASET] = _open_datasets[CLUSTER_EXTENTS_DATASET].getSpace();

       _open_datasets[IMAGE_META_DATASET]      = group->openDataSet("image_meta");
       _open_dataspaces[IMAGE_META_DATASET]    = _open_datasets[IMAGE_META_DATASET].getSpace();

       _open_datasets[VOXELS_DATASET]          = group->openDataSet("voxels");
       _open_dataspaces[VOXELS_DATASET]        = _open_datasets[VOXELS_DATASET].getSpace();

    }

    return;
  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::serialize  (H5::Group * group){
  

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


    /////////////////////////////////////////////////////////
    // Step 1: Get the current dataset dimensions
    /////////////////////////////////////////////////////////

    H5::DataSet extents_dataset = group->openDataSet("extents");
    H5::DataSpace extents_dataspace = extents_dataset.getSpace();
    // Get the dataset current size
    hsize_t extents_dims_current[1];
    extents_dataspace.getSimpleExtentDims(extents_dims_current, NULL);


    H5::DataSet projection_extents_dataset = group->openDataSet("projection_extents");
    H5::DataSpace projection_extents_dataspace = projection_extents_dataset.getSpace();
    // Get the dataset current size
    hsize_t projection_extents_dims_current[1];
    projection_extents_dataspace.getSimpleExtentDims(projection_extents_dims_current, NULL);


    H5::DataSet image_meta_dataset = group->openDataSet("image_meta");
    H5::DataSpace image_meta_dataspace = image_meta_dataset.getSpace();
    // Get the dataset current size
    hsize_t image_meta_dims_current[1];
    image_meta_dataspace.getSimpleExtentDims(image_meta_dims_current, NULL);

    H5::DataSet cluster_extents_dataset = group->openDataSet("cluster_extents");
    H5::DataSpace cluster_extents_dataspace = cluster_extents_dataset.getSpace();
    // Get the dataset current size
    hsize_t cluster_extents_dims_current[1];
    cluster_extents_dataspace.getSimpleExtentDims(cluster_extents_dims_current, NULL);


    H5::DataSet voxels_dataset = group->openDataSet("voxels");
    H5::DataSpace voxels_dataspace = voxels_dataset.getSpace();
    // Get the dataset current size
    hsize_t voxels_dims_current[1];
    voxels_dataspace.getSimpleExtentDims(voxels_dims_current, NULL);


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
    extents_dataset.extend(extents_size);

    // Create an extents object to go into the extents table:

    Extents_t next_extents;
    next_extents.first = projection_extents_dims_current[0];
    next_extents.n = projection_extents.size();


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
    // Step 4: Update the projection extents table
    /////////////////////////////////////////////////////////


    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t projection_extents_slab_dims[1];
    projection_extents_slab_dims[0] = projection_extents.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t projection_extents_size[1];
    projection_extents_size[0] = projection_extents_dims_current[0] + projection_extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    projection_extents_dataset.extend(projection_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new projection_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    projection_extents_dataspace = projection_extents_dataset.getSpace();
    projection_extents_dataspace.selectHyperslab(H5S_SELECT_SET, projection_extents_slab_dims, projection_extents_dims_current);

    // Define memory space:
    H5::DataSpace projection_extents_memspace(1, projection_extents_slab_dims);


    // Write the new data
    projection_extents_dataset.write(&(projection_extents[0]), 
      larcv3::get_datatype<IDExtents_t>(), projection_extents_memspace, projection_extents_dataspace);

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
    image_meta_dataset.write(&(image_meta[0]), 
      larcv3::ImageMeta<dimension>::get_datatype(), image_meta_memspace, image_meta_dataspace);

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
    cluster_extents_dataset.extend(cluster_extents_size);


    /////////////////////////////////////////////////////////
    // Write the new cluster_extents to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    cluster_extents_dataspace = cluster_extents_dataset.getSpace();
    cluster_extents_dataspace.selectHyperslab(H5S_SELECT_SET, cluster_extents_slab_dims, cluster_extents_dims_current);

    // Define memory space:
    H5::DataSpace cluster_extents_memspace(1, cluster_extents_slab_dims);


    // Write the new data
    cluster_extents_dataset.write(&(cluster_extents[0]), 
      larcv3::get_datatype<IDExtents_t>(), cluster_extents_memspace, cluster_extents_dataspace);


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
        voxels_dataspace = voxels_dataset.getSpace();
        voxels_dataspace.selectHyperslab(H5S_SELECT_SET,
          new_voxels_slab_dims, offset_voxels_slab_dims);

        // Define memory space:
        H5::DataSpace voxels_memspace(1, new_voxels_slab_dims);


        // Write the new data
        voxels_dataset.write(&(_cluster_v.at(projection_id).as_vector().at(cluster_id).as_vector()[0]), 
          larcv3::Voxel::get_datatype(), voxels_memspace, voxels_dataspace);

        starting_index += new_voxels_slab_dims[0];
      }
    }




    return;

  }

  template<size_t dimension> 
  void EventSparseCluster<dimension>::deserialize(H5::Group * group, size_t entry){


    // This function reads in a set of voxels for sparse clusters
    // The function implementation is:
    // 1) Read the extents table entry for this event
    // 2) Use the entry information to get the projection_extents table information
    // 3) Use the entry information to get the image_meta table information
    // 4) Use the projection_extents information to get the cluster extents information
    // 5) Use the cluster_extents information to read the correct voxels
    // 6) Update the meta for each set correctly

    // openDataSets(group);
    /////////////////////////////////////////////////////////
    // Step 1: Get the extents information from extents dataset
    /////////////////////////////////////////////////////////

    H5::DataSet * extents_dataset = &(_open_datasets[EXTENTS_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace extents_dataspace = extents_dataset.getSpace();


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
    _open_dataspaces[EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET, 
      extents_slab_dims, extents_offset);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);

    Extents_t input_extents;
    // Write the new data
    extents_dataset->read(&(input_extents), larcv3::get_datatype<Extents_t>(),
     extents_memspace, _open_dataspaces[EXTENTS_DATASET]);


    /////////////////////////////////////////////////////////
    // Step 2: Get the projection_extents information
    /////////////////////////////////////////////////////////


    // Next, open the relevant sections of the data 

    // If there are no voxels, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    H5::DataSet projection_extents_dataset = group->openDataSet("projection_extents");

    // Get a dataspace inside this file:
    H5::DataSpace projection_extents_dataspace = projection_extents_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t projection_extents_slab_dims[1];
    projection_extents_slab_dims[0] = input_extents.n;

    hsize_t projection_extents_offset[1];
    projection_extents_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    projection_extents_dataspace.selectHyperslab(H5S_SELECT_SET, 
      projection_extents_slab_dims, projection_extents_offset);


    H5::DataSpace projection_extents_memspace(1, projection_extents_slab_dims);

    std::vector<IDExtents_t> projection_extents;

    // Reserve space for reading in projection_extents:
    projection_extents.resize(input_extents.n);

    projection_extents_dataset.read(&(projection_extents[0]), 
      larcv3::get_datatype<IDExtents_t>(), projection_extents_memspace, projection_extents_dataspace);

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

    std::vector<ImageMeta<dimension> > image_meta;

    // Reserve space for reading in image_meta:
    image_meta.resize(input_extents.n);

    image_meta_dataset.read(&(image_meta[0]), 
      larcv3::ImageMeta<dimension>::get_datatype(), 
      image_meta_memspace, image_meta_dataspace);

    // std::cout << "image_meta.size(): " << image_meta.size() << std::endl;
        
    /////////////////////////////////////////////////////////
    // Step 4: Read the cluster_extents
    /////////////////////////////////////////////////////////


    H5::DataSet cluster_extents_dataset = group->openDataSet("cluster_extents");

    // Get a dataspace inside this file:
    H5::DataSpace cluster_extents_dataspace = cluster_extents_dataset.getSpace();

    // Loop over the projection extents to find out the cluster_extents:
    size_t n_total_clusters = 0;
    for (size_t projection_id = 0; projection_id < projection_extents.size(); projection_id ++)
      n_total_clusters += projection_extents.at(projection_id).n;

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t cluster_extents_slab_dims[1];
    cluster_extents_slab_dims[0] = n_total_clusters;

    hsize_t cluster_extents_offset[1];
    cluster_extents_offset[0] = projection_extents.front().first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    cluster_extents_dataspace.selectHyperslab(H5S_SELECT_SET, 
      cluster_extents_slab_dims, cluster_extents_offset);


    H5::DataSpace cluster_extents_memspace(1, cluster_extents_slab_dims);

    std::vector<IDExtents_t> cluster_extents;

    // Reserve space for reading in cluster_extents:
    cluster_extents.resize(n_total_clusters);

    cluster_extents_dataset.read(&(cluster_extents[0]), 
      larcv3::get_datatype<IDExtents_t>(), cluster_extents_memspace, cluster_extents_dataspace);

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

        // Get the dataset for reading:
        H5::DataSet voxels_dataset = group->openDataSet("voxels");

        // Get a dataspace inside this file:
        H5::DataSpace voxels_dataspace = voxels_dataset.getSpace();

        // Create a dimension for the data to add (which is the hyperslab data)
        hsize_t voxels_slab_dims[1];
        voxels_slab_dims[0] = this_cluster_extent.n;

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
