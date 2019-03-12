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

////////Serialization is hidden from SWIG:
#ifndef SWIG

  void EventParticle::serialize(H5::Group * group){


    return;
  }

  void EventParticle::initialize(H5::Group * group){

    // Initialize is ONLY meant to be called on an empty group.  So, verify this group 
    // is empty:

    if (group -> getNumObjs() != 0){
      LARCV_CRITICAL() << "Attempt to initialize non empty particle group " << group->fromClass() << std::endl;
      throw larbys();
    }

    std::cout << "Initialize Called" << std::endl;

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
    return;
  }

#endif // swig

}

#endif
