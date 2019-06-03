#ifndef __LARCV3DATAFORMAT_EVENTPARTICLE_CXX
#define __LARCV3DATAFORMAT_EVENTPARTICLE_CXX

#include "larcv3/core/dataformat/EventParticle.h"

#define PARTICLE_EXTENTS_CHUNK_SIZE 100
#define PARTICLE_DATA_CHUNK_SIZE 500
#define PARTICLE_COMPRESSION 1


#define EXTENTS_DATASET 0
#define PARTICLES_DATASET 1
#define N_DATASETS 2

namespace larcv3{
  /// Global larcv3::SBClusterFactory to register ClusterAlgoFactory
  static EventParticleFactory __global_EventParticleFactory__;

  EventParticle::EventParticle(){
    _extents_datatype = larcv3::get_datatype<Extents_t>();
    _particle_datatype = larcv3::Particle::get_datatype();
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


  void EventParticle::open_datasets(H5::Group * group){

    if (_open_datasets.size() < N_DATASETS ){
        std::cout << "Opening datasets" << std::endl;
       _open_datasets.resize(N_DATASETS);
       _open_dataspaces.resize(N_DATASETS);
       
       _open_datasets[EXTENTS_DATASET]         = group->openDataSet("extents");
       _open_dataspaces[EXTENTS_DATASET]       = _open_datasets[EXTENTS_DATASET].getSpace();

       _open_datasets[PARTICLES_DATASET]         = group->openDataSet("particles");
       _open_dataspaces[PARTICLES_DATASET]       = _open_datasets[PARTICLES_DATASET].getSpace();

    }

    return;
  }


  void EventParticle::serialize(H5::Group * group){


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

    open_datasets(group);

    /////////////////////////////////////////////////////////
    // Create the new extents object
    /////////////////////////////////////////////////////////
    Extents_t next_extents;



    /////////////////////////////////////////////////////////
    // Get and extend the particles dataset
    /////////////////////////////////////////////////////////
    H5::DataSet * particles_dataset = &(_open_datasets[PARTICLES_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace particles_dataspace = particles_dataset.getSpace();

    // Get the dataset current size
    hsize_t particles_dims_current[1];
    _open_dataspaces[PARTICLES_DATASET].getSimpleExtentDims(particles_dims_current, NULL);

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
    particles_dataset->extend(particles_size);


    /////////////////////////////////////////////////////////
    // Write the new particles to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    _open_dataspaces[PARTICLES_DATASET].selectHyperslab(H5S_SELECT_SET, 
        particles_slab_dims, particles_dims_current);

    // Define memory space:
    H5::DataSpace particles_memspace(1, particles_slab_dims);


    // Write the new data
    particles_dataset->write(&(_part_v[0]), _particle_datatype, 
        particles_memspace, _open_dataspaces[PARTICLES_DATASET]);



    /////////////////////////////////////////////////////////
    // Get the extents dataset
    /////////////////////////////////////////////////////////

    // H5::DataSet * extents_dataset = &(_open_datasets[EXTENTS_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace extents_dataspace = extents_dataset.getSpace();


    // Get the dataset current size
    hsize_t extents_dims_current[1];
    _open_dataspaces[EXTENTS_DATASET].getSimpleExtentDims(extents_dims_current, NULL);

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t extents_size[1];
    extents_size[0] = extents_dims_current[0] + extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    _open_datasets[EXTENTS_DATASET].extend(extents_size);


    /////////////////////////////////////////////////////////
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    _open_dataspaces[EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_dims_current);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);


    // Write the new data
    _open_datasets[EXTENTS_DATASET].write(&(next_extents), _extents_datatype, extents_memspace, _open_dataspaces[EXTENTS_DATASET]);


    /////////////////////////////////////////////////////////
    // Serialized!
    /////////////////////////////////////////////////////////


    return;
  }

  void EventParticle::initialize(H5::Group * group){

    // Initialize is ONLY meant to be called on an empty group.  So, verify this group 
    // is empty:

    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group->fromClass() << std::endl;
      throw larbys();
    }


    // For particles, we store a flat table of particles and an extents table.

    /////////////////////////////////////////////////////////
    // Create the extents dataset
    /////////////////////////////////////////////////////////

    // The extents table is just the first and last index of every entry's
    // particles in the data tree.


    // Get the data type for extents:
    H5::DataType extents_datatype = _extents_datatype;


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t extents_starting_dim[] = {0};
    hsize_t extents_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace extents_dataspace(1, extents_starting_dim, extents_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList extents_cparms;
    hsize_t      extents_chunk_dims[1] ={PARTICLE_EXTENTS_CHUNK_SIZE};
    extents_cparms.setChunk( 1, extents_chunk_dims );
    extents_cparms.setDeflate(PARTICLE_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet extents_ds = group->createDataSet("extents", extents_datatype, extents_dataspace, extents_cparms);


    /////////////////////////////////////////////////////////
    // Create the particles dataset
    /////////////////////////////////////////////////////////

    // The particles table is a flat table of particles, one event appended to another


    // Get the data type for extents:
    H5::DataType particle_datatype = _particle_datatype;


    // Get the starting size (0) and dimensions (unlimited)
    hsize_t particle_starting_dim[] = {0};
    hsize_t particle_maxsize_dim[]  = {H5S_UNLIMITED};

    // Create a dataspace 
    H5::DataSpace particle_dataspace(1, particle_starting_dim, particle_maxsize_dim);

    /*
     * Modify dataset creation properties, i.e. enable chunking.
     */
    H5::DSetCreatPropList particle_cparms;
    hsize_t      particle_chunk_dims[1] ={PARTICLE_DATA_CHUNK_SIZE};
    particle_cparms.setChunk( 1, particle_chunk_dims );
    particle_cparms.setDeflate(PARTICLE_COMPRESSION);

    // Create the extents dataset:
    H5::DataSet particle_ds = group->createDataSet("particles", particle_datatype, particle_dataspace, particle_cparms);


  }

  void EventParticle::deserialize(H5::Group * group, size_t entry){
    
    // Deserialization is, in some ways, easier than serialization.
    // We just have to read data from the file, and wrap it into an std::vector.

    // std::cout << "Deserialize entry " << entry << std::endl;

    /////////////////////////////////////////////////////////
    // Get the extents information from extents dataset
    /////////////////////////////////////////////////////////

    open_datasets(group);

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
    // extents_dataspace = extents_dataset.getSpace();
    _open_dataspaces[EXTENTS_DATASET].selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_offset);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);

    Extents_t input_extents;
    // Write the new data
    _open_datasets[EXTENTS_DATASET].read(&(input_extents), _extents_datatype,
      extents_memspace, _open_dataspaces[EXTENTS_DATASET]);

    // std::cout << " Extents start: " << input_extents.first << ", end: "
    //           << input_extents.first + input_extents.n << std::endl;


    // Next, open the relevant sections of the data 

    // If there are no particles, dont read anything:
    if ( input_extents.n == 0){
        return;
    }

    // H5::DataSet * particles_dataset = &(_open_datasets[PARTICLES_DATASET]);

    // Get a dataspace inside this file:
    // H5::DataSpace particles_dataspace = particles_dataset.getSpace();

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t particles_slab_dims[1];
    particles_slab_dims[0] = input_extents.n;

    hsize_t particles_offset[1];
    particles_offset[0] = input_extents.first;

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    _open_dataspaces[PARTICLES_DATASET].selectHyperslab(H5S_SELECT_SET, particles_slab_dims, particles_offset);


    H5::DataSpace particles_memspace(1, particles_slab_dims);

    // Reserve space for reading in particles:
    _part_v.resize(input_extents.n);

    _open_datasets[PARTICLES_DATASET].read(&(_part_v[0]), _particle_datatype,
        particles_memspace, _open_dataspaces[PARTICLES_DATASET]);

    

    return;
  }

// #endif // swig

} // larcv3

#endif
