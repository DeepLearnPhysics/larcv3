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

#endif
