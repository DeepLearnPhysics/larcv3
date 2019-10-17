#ifndef __LARCV3DATAFORMAT_EVENTPARTICLE_CXX
#define __LARCV3DATAFORMAT_EVENTPARTICLE_CXX

#include "larcv3/core/dataformat/EventParticle.h"

#define PARTICLE_EXTENTS_CHUNK_SIZE 100
#define PARTICLE_DATA_CHUNK_SIZE 500


#define EXTENTS_DATASET 0
#define PARTICLES_DATASET 1
#define N_DATASETS 2

namespace larcv3{
  /// Global larcv3::SBClusterFactory to register ClusterAlgoFactory
  static EventParticleFactory __global_EventParticleFactory__;

  EventParticle::EventParticle(){

    _data_types.resize(N_DATASETS);
    _data_types[EXTENTS_DATASET] = larcv3::get_datatype<Extents_t>();
    _data_types[PARTICLES_DATASET] = larcv3::Particle::get_datatype();


  }



  void EventParticle::set(const std::vector<larcv3::Particle>& part_v)
  {
    _part_v = part_v;
    for(size_t i=0; i<_part_v.size(); ++i)
      _part_v[i].id(i);
  }

  void EventParticle::append(const larcv3::Particle& part)
  {
    _part_v.push_back(part);
    _part_v.back().id(_part_v.size()-1);
  }

  void EventParticle::emplace_back(larcv3::Particle&& part)
  {
    _part_v.emplace_back(std::move(part));
    _part_v.back().id(_part_v.size()-1);
  }

  void EventParticle::emplace(std::vector<larcv3::Particle>&& part_v)
  {
    _part_v = std::move(part_v);
    for(size_t i=0; i<_part_v.size(); ++i)
      _part_v[i].id(i);
  }

  void EventParticle::clear(){ 
    _part_v.clear();
  }
////////Serialization is hidden from SWIG:
// #ifndef SWIG


  void EventParticle::finalize(){
    for (size_t i =0; i < _open_in_datasets.size(); i ++){
      H5Dclose(_open_in_datasets[i]);
    }
    for (size_t i =0; i < _open_out_datasets.size(); i ++){
      H5Dclose(_open_out_datasets[i]);
    }
  }

  void EventParticle::open_in_datasets(hid_t group){

    if (_open_in_datasets.size() < N_DATASETS ){
       _open_in_datasets.resize(N_DATASETS);
       _open_in_dataspaces.resize(N_DATASETS);

       _open_in_datasets[EXTENTS_DATASET]         = H5Dopen(group, "extents", H5P_DEFAULT);
       _open_in_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_in_datasets[EXTENTS_DATASET]);

       _open_in_datasets[PARTICLES_DATASET]       = H5Dopen(group, "particles", H5P_DEFAULT);
       _open_in_dataspaces[PARTICLES_DATASET]     = H5Dget_space(_open_in_datasets[PARTICLES_DATASET]);

    }

    return;
  }

  void EventParticle::open_out_datasets(hid_t group){

    if (_open_out_datasets.size() < N_DATASETS ){
       _open_out_datasets.resize(N_DATASETS);
       _open_out_dataspaces.resize(N_DATASETS);
       
       _open_out_datasets[EXTENTS_DATASET]         = H5Dopen(group, "extents", H5P_DEFAULT);
       _open_out_dataspaces[EXTENTS_DATASET]       = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);

       _open_out_datasets[PARTICLES_DATASET]       = H5Dopen(group, "particles", H5P_DEFAULT);
       _open_out_dataspaces[PARTICLES_DATASET]     = H5Dget_space(_open_out_datasets[PARTICLES_DATASET]);

    }

    return;
  }

  void EventParticle::serialize(hid_t group){


    // Serialization is a multi step process.  
    // - First, we update the data table.
    //  - fetch the dataset from the group
    //  - Note the total size of the dataset as the 'first' extents object
    //  - extend the dataset by _part_v.size() elements
    //  - Note 'first' + _part_v.size() as the 'last' extents object
    //  - select a hyperslab of the last _part_v.size() elements
    //  - write the data to the group
    // - Create an extents object from first and last
    // - Get the extents dataset.
    //  - Extend the extents dataset by 1
    //  - Append the latest extents
    // Return

    open_out_datasets(group);

    /////////////////////////////////////////////////////////
    // Create the new extents object
    /////////////////////////////////////////////////////////
    Extents_t next_extents;



    /////////////////////////////////////////////////////////
    // Get and extend the particles dataset
    /////////////////////////////////////////////////////////
    // H5::DataSet * particles_dataset = &(_open_datasets[PARTICLES_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace particles_dataspace = particles_dataset.getSpace();

    // Get the dataset current size
    hsize_t particles_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[PARTICLES_DATASET], particles_dims_current, NULL);
    // .getSimpleExtentDims(particles_dims_current, NULL);

    // Make a note of the first index:
    next_extents.first = particles_dims_current[0];

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t particles_slab_dims[1];
    particles_slab_dims[0] = _part_v.size();


    // Create a size vector for the FULL dataset: previous + current
    hsize_t particles_size[1];
    particles_size[0] = particles_dims_current[0] + particles_slab_dims[0];

    // Make a note of the last index:
    next_extents.n = _part_v.size();


    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[PARTICLES_DATASET], particles_size);
    // .extend(particles_size);


    /////////////////////////////////////////////////////////
    // Write the new particles to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    // Need to reopen the dataspace after extension:
    _open_out_dataspaces[PARTICLES_DATASET] = H5Dget_space(_open_out_datasets[PARTICLES_DATASET]);

    H5Sselect_hyperslab(_open_out_dataspaces[PARTICLES_DATASET], 
      H5S_SELECT_SET, 
      particles_dims_current, // start
      NULL ,                  // stride
      particles_slab_dims,    // count 
      NULL                    // block
      );


    // Define memory space:
    // H5::DataSpace particles_memspace(1, particles_slab_dims);
    hid_t particles_memspace = H5Screate_simple(1, particles_slab_dims, NULL);

    // Transfer property list, default:
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);

    // Write the new data
    // _open_out_datasets[PARTICLES_DATASET].write(&(_part_v[0]), _data_types[PARTICLES_DATASET], 
        // particles_memspace, _open_out_dataspaces[PARTICLES_DATASET]);

    H5Dwrite(_open_out_datasets[PARTICLES_DATASET],   // dataset_id,
             _data_types[PARTICLES_DATASET],          // hit_t mem_type_id, 
             particles_memspace,                      // hid_t mem_space_id, 
             _open_out_dataspaces[PARTICLES_DATASET], //hid_t file_space_id, 
             xfer_plist_id,                           //hid_t xfer_plist_id, 
             &(_part_v[0])                            // const void * buf 
           );

    /////////////////////////////////////////////////////////
    // Get the extents dataset
    /////////////////////////////////////////////////////////

    // H5::DataSet * extents_dataset = &(_open_out_datasets[EXTENTS_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace extents_dataspace = extents_dataset.getSpace();


    // Get the dataset current size
     hsize_t extents_dims_current[1];
    H5Sget_simple_extent_dims(_open_out_dataspaces[EXTENTS_DATASET], extents_dims_current, NULL);

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t extents_size[1];
    extents_size[0] = extents_dims_current[0] + extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    H5Dset_extent(_open_out_datasets[EXTENTS_DATASET], extents_size);


    /////////////////////////////////////////////////////////
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////



    // Select as a hyperslab the last section of data for writing:
    // Need to reopen the dataspace after extension:
    _open_out_dataspaces[EXTENTS_DATASET] = H5Dget_space(_open_out_datasets[EXTENTS_DATASET]);

    H5Sselect_hyperslab(_open_out_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_dims_current, // start
      NULL ,  // stride
      extents_slab_dims, //count 
      NULL // block
      );


    // Define memory space:
    // H5::DataSpace particles_memspace(1, particles_slab_dims);
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);

    // Write the new data
    // _open_out_datasets[PARTICLES_DATASET].write(&(_part_v[0]), _data_types[PARTICLES_DATASET], 
        // extents_memspace, _open_out_dataspaces[PARTICLES_DATASET]);

    H5Dwrite(_open_out_datasets[EXTENTS_DATASET], // dataset_id,
             _data_types[EXTENTS_DATASET], // hit_t mem_type_id, 
             extents_memspace, // hid_t mem_space_id, 
             _open_out_dataspaces[EXTENTS_DATASET], //hid_t file_space_id, 
             xfer_plist_id, //hid_t xfer_plist_id, 
             &(next_extents) // const void * buf 
             );


    /////////////////////////////////////////////////////////
    // Serialized!
    /////////////////////////////////////////////////////////


    return;
  }

  void EventParticle::initialize(hid_t group, uint compression){

    // Initialize is ONLY meant to be called on an empty group.  So, verify this group 
    // is empty:

    if (get_num_objects(group) != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group << std::endl;
      throw larbys();
    }


    // For particles, we store a flat table of particles and an extents table.

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
    // H5::DataSpace extents_dataspace(1, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    hid_t extents_cparms = H5Pcreate( H5P_DATASET_CREATE );
    // H5::DSetCreatPropList extents_cparms;
    hsize_t      extents_chunk_dims[1] ={PARTICLE_EXTENTS_CHUNK_SIZE};
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
    // H5::DataSet extents_ds = group->createDataSet("extents", 
        // *_data_types[EXTENTS_DATASET], extents_dataspace, extents_cparms);


    /////////////////////////////////////////////////////////
    // Create the particles dataset
    /////////////////////////////////////////////////////////

    // The particles table is a flat table of particles, one event appended to another


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t particle_starting_dim[] = {0};
    hsize_t particle_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    hid_t particle_dataspace = H5Screate_simple(1, particle_starting_dim, particle_maxsize_dim);
    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */

    hid_t   particle_cparms = H5Pcreate( H5P_DATASET_CREATE );
    hsize_t particle_chunk_dims[1] ={PARTICLE_DATA_CHUNK_SIZE};
    
    H5Pset_chunk(particle_cparms, 1, particle_chunk_dims );
    if (compression){
      H5Pset_deflate(particle_cparms, compression);
    }

    // Create the extents dataset:
    H5Dcreate(
      group,                          // hid_t loc_id  IN: Location identifier
      "particles",                    // const char *name      IN: Dataset name
      _data_types[PARTICLES_DATASET], // hid_t dtype_id  IN: Datatype identifier
      particle_dataspace,             // hid_t space_id  IN: Dataspace identifier
      NULL,                           // hid_t lcpl_id IN: Link creation property list
      particle_cparms,                // hid_t dcpl_id IN: Dataset creation property list
      NULL                            // hid_t dapl_id IN: Dataset access property list
      );

  }

  void EventParticle::deserialize(hid_t group, size_t entry, bool reopen_groups){
    
    // Deserialization is, in some ways, easier than serialization.
    // We just have to read data from the file, and wrap it into an std::vector.

    // std::cout << "Deserialize entry " << entry << std::endl;

    /////////////////////////////////////////////////////////
    // Get the extents information from extents dataset
    /////////////////////////////////////////////////////////
    
    if (reopen_groups){
      _open_in_dataspaces.clear();
      _open_in_datasets.clear();
    }

    open_in_datasets(group);

    // H5::DataSet * extents_dataset = &(_open_datasets[EXTENTS_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace extents_dataspace = extents_dataset.getSpace();


    // // Get the dataset current size
    // hsize_t extents_dims_current[1];
    // extents_dataspace.getSimpleExtentDims(extents_dims_current, NULL);

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;

    hsize_t extents_offset[1];
    extents_offset[0] = entry;


    // // Create a size vector for the FULL dataset: previous + current
    // hsize_t extents_size[1];
    // extents_size[0] = extents_dims_current[0] + extents_slab_dims[0];

    // // Extend the dataset to accomodate the new data
    // extents_dataset.extend(extents_size);


    /////////////////////////////////////////////////////////
    // Read this extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    
    H5Sselect_hyperslab(_open_in_dataspaces[EXTENTS_DATASET], 
      H5S_SELECT_SET, 
      extents_offset, // start
      NULL ,  // stride
      extents_slab_dims, //count 
      NULL // block
      );

    // Define memory space:
    hid_t extents_memspace = H5Screate_simple(1, extents_slab_dims, NULL);

    Extents_t input_extents;
    // Write the new data
    // _open_in_datasets[EXTENTS_DATASET].read(&(input_extents), *_data_types[EXTENTS_DATASET],
      // extents_memspace, _open_in_dataspaces[EXTENTS_DATASET]);
    
    // Transfer property list, default
    hid_t xfer_plist_id = H5Pcreate(H5P_DATASET_XFER);

    H5Dread(
      _open_in_datasets[EXTENTS_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[EXTENTS_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      extents_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[EXTENTS_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                         // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(input_extents)                       // void * buf  OUT: Buffer to receive data read from file.
    );

    // std::cout << " Extents start: " << input_extents.first << ", end: "
    //           << input_extents.first + input_extents.n << std::endl;


    // Next, open the relevant sections of the data 

    // If there are no particles, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    // H5::DataSet * particles_dataset = &(_open_in_datasets[PARTICLES_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace particles_dataspace = particles_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t particles_slab_dims[1];
    particles_slab_dims[0] = input_extents.n;

    hsize_t particles_offset[1];
    particles_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    // _open_in_dataspaces[PARTICLES_DATASET].selectHyperslab(H5S_SELECT_SET, particles_slab_dims, particles_offset);

    H5Sselect_hyperslab(_open_in_dataspaces[PARTICLES_DATASET], 
      H5S_SELECT_SET, 
      particles_offset,    // start
      NULL ,               // stride
      particles_slab_dims, //count 
      NULL                 // block
      );

    // H5::DataSpace particles_memspace(1, particles_slab_dims);

    // Define memory space:
    hid_t particles_memspace = H5Screate_simple(1, particles_slab_dims, NULL);

    // Reserve space for reading in particles:
    _part_v.resize(input_extents.n);

    // _open_in_datasets[PARTICLES_DATASET].read(&(_part_v[0]), *_data_types[PARTICLES_DATASET],
    //     particles_memspace, _open_in_dataspaces[PARTICLES_DATASET]);

    
    H5Dread(
      _open_in_datasets[PARTICLES_DATASET],    // hid_t dataset_id  IN: Identifier of the dataset read from.
      _data_types[PARTICLES_DATASET],          // hid_t mem_type_id IN: Identifier of the memory datatype.
      particles_memspace,                      // hid_t mem_space_id  IN: Identifier of the memory dataspace.
      _open_in_dataspaces[PARTICLES_DATASET],  // hid_t file_space_id IN: Identifier of the dataset's dataspace in the file.
      xfer_plist_id,                         // hid_t xfer_plist_id     IN: Identifier of a transfer property list for this I/O operation.
      &(_part_v[0])                       // void * buf  OUT: Buffer to receive data read from file.
    );

    return;
  }

// #endif // swig

} // larcv3

#endif
