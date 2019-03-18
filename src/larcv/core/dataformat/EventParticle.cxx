#ifndef __LARCV_EVENTPARTICLE_CXX
#define __LARCV_EVENTPARTICLE_CXX

#include "EventParticle.h"

#define PARTICLE_EXTENTS_CHUNK_SIZE 1
#define PARTICLE_DATA_CHUNK_SIZE 5

namespace larcv{
  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static EventParticleFactory __global_EventParticleFactory__;


  void EventParticle::set(const std::vector<larcv::Particle>& part_v)
  {
    _part_v = part_v;
    for(size_t i=0; i<_part_v.size(); ++i)
      _part_v[i].id(i);
  }

  void EventParticle::append(const larcv::Particle& part)
  {
    _part_v.push_back(part);
    _part_v.back().id(_part_v.size()-1);
  }

  void EventParticle::emplace_back(larcv::Particle&& part)
  {
    _part_v.emplace_back(std::move(part));
    _part_v.back().id(_part_v.size()-1);
  }

  void EventParticle::emplace(std::vector<larcv::Particle>&& part_v)
  {
    _part_v = std::move(part_v);
    for(size_t i=0; i<_part_v.size(); ++i)
      _part_v[i].id(i);
  }

  void EventParticle::clear(){ 
    std::cout<< "Clearing" << std::endl; _part_v.clear();
  }
////////Serialization is hidden from SWIG:
// #ifndef SWIG

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


    /////////////////////////////////////////////////////////
    // Create the new extents object
    /////////////////////////////////////////////////////////
    Extents_t next_extents;



    /////////////////////////////////////////////////////////
    // Get and extend the particles dataset
    /////////////////////////////////////////////////////////
    H5::DataSet particles_dataset = group->openDataSet("particles");

    // Get a dataspace inside this file:
    H5::DataSpace particles_dataspace = particles_dataset.getSpace();

    // Get the dataset current size
    hsize_t particles_dims_current[1];
    particles_dataspace.getSimpleExtentDims(particles_dims_current, NULL);

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
    particles_dataset.extend(particles_size);


    /////////////////////////////////////////////////////////
    // Write the new particles to the dataset
    /////////////////////////////////////////////////////////

    // Select as a hyperslab the last section of data for writing:
    particles_dataspace = particles_dataset.getSpace();
    particles_dataspace.selectHyperslab(H5S_SELECT_SET, particles_slab_dims, particles_dims_current);

    // Define memory space:
    H5::DataSpace particles_memspace(1, particles_slab_dims);


    // Write the new data
    particles_dataset.write(&(_part_v[0]), larcv::Particle::get_datatype(), particles_memspace, particles_dataspace);



    /////////////////////////////////////////////////////////
    // Get the extents dataset
    /////////////////////////////////////////////////////////

    H5::DataSet extents_dataset = group->openDataSet("extents");

    // Get a dataspace inside this file:
    H5::DataSpace extents_dataspace = extents_dataset.getSpace();


    // Get the dataset current size
    hsize_t extents_dims_current[1];
    extents_dataspace.getSimpleExtentDims(extents_dims_current, NULL);

    // Create a dimension for the data to add (which is the hyperslab data)
    hsize_t extents_slab_dims[1];
    extents_slab_dims[0] = 1;


    // Create a size vector for the FULL dataset: previous + current
    hsize_t extents_size[1];
    extents_size[0] = extents_dims_current[0] + extents_slab_dims[0];

    // Extend the dataset to accomodate the new data
    extents_dataset.extend(extents_size);


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
    hsize_t      extents_chunk_dims[1] ={PARTICLE_EXTENTS_CHUNK_SIZE};
    extents_cparms.setChunk( 1, extents_chunk_dims );

    // Create the extents dataset:
    H5::DataSet extents_ds = group->createDataSet("extents", extents_datatype, extents_dataspace, extents_cparms);


    /////////////////////////////////////////////////////////
    // Create the particles dataset
    /////////////////////////////////////////////////////////

    // The particles table is a flat table of particles, one event appended to another


    // Get the data type for extents:
    H5::DataType particle_datatype = larcv::Particle::get_datatype();


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

    // Create the extents dataset:
    H5::DataSet particle_ds = group->createDataSet("particles", particle_datatype, particle_dataspace, particle_cparms);


  }

  void EventParticle::deserialize(H5::Group * group, size_t entry){
    
    // Deserialization is, in some ways, easier than serialization.
    // We just have to read data from the file, and wrap it into an std::vector.

 

    /////////////////////////////////////////////////////////
    // Get the extents information from extents dataset
    /////////////////////////////////////////////////////////

    H5::DataSet extents_dataset = group->openDataSet("extents");

    // Get a dataspace inside this file:
    H5::DataSpace extents_dataspace = extents_dataset.getSpace();


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
    // Write the new extents entry to the dataset
    /////////////////////////////////////////////////////////

    // Now, select as a hyperslab the last section of data for writing:
    // extents_dataspace = extents_dataset.getSpace();
    extents_dataspace.selectHyperslab(H5S_SELECT_SET, extents_slab_dims, extents_offset);

    // Define memory space:
    H5::DataSpace extents_memspace(1, extents_slab_dims);

    Extents_t input_extents;
    // Write the new data
    extents_dataset.read(&(input_extents), larcv::get_datatype<Extents_t>(), extents_memspace, extents_dataspace);




    return;
  }

// #endif // swig

} // larcv

#endif
