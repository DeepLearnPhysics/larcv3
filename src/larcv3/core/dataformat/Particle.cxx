#ifndef __LARCV3DATAFORMAT_PARTICLE_CXX__
#define __LARCV3DATAFORMAT_PARTICLE_CXX__

#include "larcv3/core/dataformat/Particle.h"
#include <set>
#include <sstream>
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/base/larbys.h"

namespace larcv3 {

  // const BBox2D& Particle::boundingbox_2d(ProjectionID_t id) const
  // {
  //   std::stringstream ss;
  //   if(id < _bb2d_v.size()) return _bb2d_v[id];
  //   ss << "BBox2D not stored for ProjectionID_t " << id << std::endl;
  //   throw larbys(ss.str());
  // }

  std::string Particle::dump() const
  {
    std::stringstream ss;
    std::stringstream buf;
    ss  << "      \033[95m" << "Particle " << " (PdgCode,TrackID) = (" << pdg_code() << "," << track_id() << ")\033[00m "
	<< "... with Parent (" << parent_pdg_code() << "," << parent_track_id() << ")" << std::endl;
    buf << "      ";

    ss << buf.str() << "Vertex   (x, y, z, t) = (" << x() << "," << y() << "," << z() << "," << t() << ")" << std::endl
       << buf.str() << "Momentum (px, py, pz) = (" << px() << "," << py() << "," << pz() << ")" << std::endl
       << buf.str() << "Inittial Energy  = " << energy_init() << std::endl
       << buf.str() << "Deposit  Energy  = " << energy_deposit() << std::endl
       << buf.str() << "Creation Process = " << creation_process() << std::endl;

    // ss << buf.str() << "BBox3D: " << _bb3d.dump();
    // buf << "    ";
    // for(size_t i=0; i<_bb2d_v.size(); ++i)
    //   ss << buf.str() << "Plane " << i << " BBox2D: " << _bb2d_v[i].dump();

    return ss.str();
  }

  void Particle::creation_process (const std::string& proc) { 
    if (proc.size() < PARTICLE_PROCESS_STRLEN){
      std::copy(proc.begin(), proc.end(),_process); 
    }
    else{
      LARCV_CRITICAL() << "Can not use a string longer than 64 characters in Particle creation_process!" << std::endl;
      throw larbys();
    }
  }
  std::string Particle::creation_process() const { 
    return std::string(_process); 
  }


}

void init_particle(pybind11::module m){
    pybind11::class_<larcv3::Particle> particle(m, "Particle");

    particle.def(pybind11::init<>());

    particle.def("id", (larcv3::ParticleIndex_t (larcv3::Particle::*)() const)(&larcv3::Particle::id));
    particle.def("id", (void (larcv3::Particle::*)(larcv3::InstanceID_t))(&larcv3::Particle::id));

    particle.def("shape", (larcv3::ShapeType_t (larcv3::Particle::*)() const)(&larcv3::Particle::shape));
    particle.def("shape", (void (larcv3::Particle::*)(larcv3::ShapeType_t))(&larcv3::Particle::shape));

    particle.def("mcst_index", (larcv3::MCSTIndex_t (larcv3::Particle::*)() const)(&larcv3::Particle::mcst_index));
    particle.def("mcst_index", (void (larcv3::Particle::*)(larcv3::MCSTIndex_t))(&larcv3::Particle::mcst_index));

    particle.def("mct_index", (larcv3::MCTIndex_t (larcv3::Particle::*)() const)(&larcv3::Particle::mct_index));
    particle.def("mct_index", (void (larcv3::Particle::*)(larcv3::MCTIndex_t))(&larcv3::Particle::mct_index));

    particle.def("nu_current_type", (short (larcv3::Particle::*)() const)(&larcv3::Particle::nu_current_type));
    particle.def("nu_current_type", (void (larcv3::Particle::*)(short))(&larcv3::Particle::nu_current_type));
    
    particle.def("nu_interaction_type", (short (larcv3::Particle::*)() const)(&larcv3::Particle::nu_interaction_type));
    particle.def("nu_interaction_type", (void (larcv3::Particle::*)(short))(&larcv3::Particle::nu_interaction_type));
    
    particle.def("track_id", (unsigned int (larcv3::Particle::*)() const)(&larcv3::Particle::track_id));
    particle.def("track_id", (void (larcv3::Particle::*)(unsigned int))(&larcv3::Particle::track_id));


    particle.def("pdg_code", (int (larcv3::Particle::*)() const)(&larcv3::Particle::pdg_code));
    particle.def("pdg_code", (void (larcv3::Particle::*)(int))(&larcv3::Particle::pdg_code));
    
    particle.def("momentum", &larcv3::Particle::momentum);

    particle.def("px", &larcv3::Particle::px);
    particle.def("py", &larcv3::Particle::py);
    particle.def("pz", &larcv3::Particle::pz);
    particle.def("p",  &larcv3::Particle::p);
    
    particle.def("position", (const larcv3::Vertex& (larcv3::Particle::*)() const)(&larcv3::Particle::position));
    particle.def("position", (void (larcv3::Particle::*)(const larcv3::Vertex&))(&larcv3::Particle::position));
    particle.def("position", (void (larcv3::Particle::*)(double,double,double,double))(&larcv3::Particle::position));

    particle.def("x", &larcv3::Particle::x);
    particle.def("y", &larcv3::Particle::y);
    particle.def("z", &larcv3::Particle::z);
    particle.def("t", &larcv3::Particle::t);

    particle.def("end_position", (const larcv3::Vertex& (larcv3::Particle::*)() const)(&larcv3::Particle::end_position));
    particle.def("end_position", (void (larcv3::Particle::*)(const larcv3::Vertex&))(&larcv3::Particle::end_position));
    particle.def("end_position", (void (larcv3::Particle::*)(double,double,double,double))(&larcv3::Particle::end_position));

    particle.def("first_step", (const larcv3::Vertex& (larcv3::Particle::*)() const)(&larcv3::Particle::first_step));
    particle.def("first_step", (void (larcv3::Particle::*)(const larcv3::Vertex&))(&larcv3::Particle::first_step));
    particle.def("first_step", (void (larcv3::Particle::*)(double,double,double,double))(&larcv3::Particle::first_step));

    particle.def("last_step", (const larcv3::Vertex& (larcv3::Particle::*)() const)(&larcv3::Particle::last_step));
    particle.def("last_step", (void (larcv3::Particle::*)(const larcv3::Vertex&))(&larcv3::Particle::last_step));
    particle.def("last_step", (void (larcv3::Particle::*)(double,double,double,double))(&larcv3::Particle::last_step));

    
    particle.def("distance_travel", (double (larcv3::Particle::*)() const)(&larcv3::Particle::distance_travel));
    particle.def("distance_travel", (void (larcv3::Particle::*)(double))(&larcv3::Particle::distance_travel));

    particle.def("energy_init", (double (larcv3::Particle::*)() const)(&larcv3::Particle::energy_init));
    particle.def("energy_init", (void (larcv3::Particle::*)(double))(&larcv3::Particle::energy_init));

    particle.def("energy_deposit", (double (larcv3::Particle::*)() const)(&larcv3::Particle::energy_deposit));
    particle.def("energy_deposit", (void (larcv3::Particle::*)(double))(&larcv3::Particle::energy_deposit));

    particle.def("creation_process", (std::string (larcv3::Particle::*)() const)(&larcv3::Particle::creation_process));
    particle.def("creation_process", (void (larcv3::Particle::*)(const std::string & ))(&larcv3::Particle::creation_process));

    particle.def("parent_track_id", (unsigned int (larcv3::Particle::*)() const)(&larcv3::Particle::parent_track_id));
    particle.def("parent_track_id", (void (larcv3::Particle::*)( unsigned int  ))(&larcv3::Particle::parent_track_id));

    particle.def("parent_pdg_code", (int (larcv3::Particle::*)() const)(&larcv3::Particle::parent_pdg_code));
    particle.def("parent_pdg_code", (void (larcv3::Particle::*)( int  ))(&larcv3::Particle::parent_pdg_code));


    particle.def("parent_position", (const larcv3::Vertex& (larcv3::Particle::*)() const)(&larcv3::Particle::parent_position));
    particle.def("parent_position", (void (larcv3::Particle::*)(const larcv3::Vertex&))(&larcv3::Particle::parent_position));
    particle.def("parent_position", (void (larcv3::Particle::*)(double,double,double,double))(&larcv3::Particle::parent_position));


    particle.def("ancestor_track_id", (unsigned int (larcv3::Particle::*)() const)(&larcv3::Particle::ancestor_track_id));
    particle.def("ancestor_track_id", (void (larcv3::Particle::*)( unsigned int  ))(&larcv3::Particle::ancestor_track_id));

    particle.def("ancestor_pdg_code", (int (larcv3::Particle::*)() const)(&larcv3::Particle::ancestor_pdg_code));
    particle.def("ancestor_pdg_code", (void (larcv3::Particle::*)( int  ))(&larcv3::Particle::ancestor_pdg_code));

    particle.def("ancestor_position", (const larcv3::Vertex& (larcv3::Particle::*)() const)(&larcv3::Particle::ancestor_position));
    particle.def("ancestor_position", (void (larcv3::Particle::*)(const larcv3::Vertex&))(&larcv3::Particle::ancestor_position));
    particle.def("ancestor_position", (void (larcv3::Particle::*)(double,double,double,double))(&larcv3::Particle::ancestor_position));

    particle.def("parent_x",   &larcv3::Particle::parent_x);
    particle.def("parent_y",   &larcv3::Particle::parent_y);
    particle.def("parent_z",   &larcv3::Particle::parent_z);
    particle.def("parent_t",   &larcv3::Particle::parent_t);
    particle.def("ancestor_x", &larcv3::Particle::ancestor_x);
    particle.def("ancestor_y", &larcv3::Particle::ancestor_y);
    particle.def("ancestor_z", &larcv3::Particle::ancestor_z);
    particle.def("ancestor_t", &larcv3::Particle::ancestor_t);
    

    particle.def("dump",       &larcv3::Particle::dump);
    particle.def("__repr__",   &larcv3::Particle::dump);
  /*


    // ancestor info setter

    std::string dump() const;

  */
}

#endif
