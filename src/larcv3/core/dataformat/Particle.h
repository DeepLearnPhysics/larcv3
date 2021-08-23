/**
 * \file Particle.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class larcv3::Particle
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_PARTICLE_H__
#define __LARCV3DATAFORMAT_PARTICLE_H__

#include <iostream>
#include "larcv3/core/dataformat/Vertex.h"
// #include "BBox.h"
#include "larcv3/core/dataformat/DataFormatTypes.h"

// In hdf5, strings are fixed length so we set the maximum length here:
#define PARTICLE_PROCESS_STRLEN 64

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#endif

namespace larcv3 {

  struct ParticleHolder{


    InstanceID_t _id; ///< "ID" of this particle
    /// index number in the origin MCShower/MCTrack container array (kINVALID_USHORT if neither)
    MCSTIndex_t  _mcst_index;
    ///< index number in the origin MCTruth container array (kINVALID_USHORT if MCShower/MCTrack)
    MCTIndex_t   _mct_index;
    ShapeType_t  _shape;       ///< shows if it is (e+/e-/gamma) or other particle types
    short _current_type;       ///< if neutrino, shows interaction GENIE current type. else kINVALID_USHORT
    short _interaction_type;   ///< if neutrino, shows interaction GENIE code. else kINVALID_USHORT

    unsigned int _trackid;     ///< Geant4 track id
    int          _pdg;         ///< PDG code
    double       _px,_py,_pz;  ///< (x,y,z) component of particle's initial momentum
    Vertex       _vtx;         ///< (x,y,z,t) of particle's vertex information
    Vertex       _end_pt;      ///< (x,y,z,t) at which particle disappeared from G4WorldVolume
    Vertex       _first_step;  ///< (x,y,z,t) of the first energy deposition point in the detector
    Vertex       _last_step;   ///< (x,y,z,t) of the last energy deposition point in the detector
    double       _dist_travel; ///< filled only if MCTrack origin: distance measured along the trajectory
    double       _energy_init; ///< initial energy of the particle
    double       _energy_deposit; ///< deposited energy of the particle in the detector
    char         _process[PARTICLE_PROCESS_STRLEN];     ///< string identifier of the particle's creation process from Geant4
    // std::vector<larcv3::BBox2D> _bb2d_v; ///< bounding box of particle's trajectory in 2D projections. index = ProjectionID_t
    // larcv3::BBox3D _bb3d; ///< bounding box of particle's trajectory in 3D
    // int _num_voxels; ///< Number of voxels in the particle's 3D cluster.

    unsigned int _parent_trackid; ///< Geant4 track id of the parent particle
    int          _parent_pdg;     ///< PDG code of the parent particle
    Vertex       _parent_vtx; ///< (x,y,z,t) of parent's vertex information

    unsigned int _ancestor_trackid; ///< Geant4 track id of the ancestor particle
    int          _ancestor_pdg;     ///< PDG code of the ancestor particle
    Vertex       _ancestor_vtx; ///< (x,y,z,t) of ancestor's vertex information
  };

  /**
     \class Particle
     \brief Particle/Interaction-wise truth information data
  */
  class Particle{

  public:

    /// Default constructor
    Particle(larcv3::ShapeType_t shape=larcv3::kShapeUnknown);



    /// Default destructor
    ~Particle(){}
    /// particle's ID getter
    inline ParticleIndex_t id      () const { return _particle_holder._id;         }
    // origin/generator info getter
    inline ShapeType_t  shape      () const { return _particle_holder._shape;      }
    inline MCSTIndex_t  mcst_index () const { return _particle_holder._mcst_index; }
    inline MCTIndex_t   mct_index  () const { return _particle_holder._mct_index;  }
    inline short nu_current_type     () const { return _particle_holder._current_type; }
    inline short nu_interaction_type () const { return _particle_holder._interaction_type; }
    // particle's info getter
    inline unsigned int track_id   () const { return _particle_holder._trackid;    }
    inline int          pdg_code   () const { return _particle_holder._pdg;        }
    inline double       px         () const { return _particle_holder._px;         }
    inline double       py         () const { return _particle_holder._py;         }
    inline double       pz         () const { return _particle_holder._pz;         }
    inline double       p          () const { return sqrt(pow(_particle_holder._px,2)+pow(_particle_holder._py,2)+pow(_particle_holder._pz,2)); }
    inline const larcv3::Vertex& position() const { return _particle_holder._vtx;   }
    inline double       x          () const { return _particle_holder._vtx.x();    }
    inline double       y          () const { return _particle_holder._vtx.y();    }
    inline double       z          () const { return _particle_holder._vtx.z();    }
    inline double       t          () const { return _particle_holder._vtx.t();    }
    inline const larcv3::Vertex& end_position () const { return _particle_holder._end_pt;     }
    inline const larcv3::Vertex& first_step   () const { return _particle_holder._first_step; }
    inline const larcv3::Vertex& last_step    () const { return _particle_holder._last_step;  }
    inline double       distance_travel () const { return _particle_holder._dist_travel;     }
    inline double       energy_init     () const { return _particle_holder._energy_init;     }
    inline double       energy_deposit  () const { return _particle_holder._energy_deposit;  }
           std::string  creation_process() const;

    // const BBox2D& boundingbox_2d(ProjectionID_t id) const;
    // inline const std::vector<larcv3::BBox2D>& boundingbox_2d() const { return _bb2d_v; }
    // inline const BBox3D& boundingbox_3d() const { return _bb3d; }

    // inline int num_voxels() const { return _num_voxels; }

    // parent info getter
    inline unsigned int parent_track_id () const { return _particle_holder._parent_trackid; }
    inline int          parent_pdg_code () const { return _particle_holder._parent_pdg;     }
    inline const larcv3::Vertex& parent_position () const { return _particle_holder._parent_vtx;}
    inline double       parent_x        () const { return _particle_holder._parent_vtx.x(); }
    inline double       parent_y        () const { return _particle_holder._parent_vtx.y(); }
    inline double       parent_z        () const { return _particle_holder._parent_vtx.z(); }
    inline double       parent_t        () const { return _particle_holder._parent_vtx.t(); }

    // ancestor info getter
    inline unsigned int ancestor_track_id () const { return _particle_holder._ancestor_trackid; }
    inline int          ancestor_pdg_code () const { return _particle_holder._ancestor_pdg;     }
    inline const larcv3::Vertex& ancestor_position () const { return _particle_holder._ancestor_vtx;}
    inline double       ancestor_x        () const { return _particle_holder._ancestor_vtx.x(); }
    inline double       ancestor_y        () const { return _particle_holder._ancestor_vtx.y(); }
    inline double       ancestor_z        () const { return _particle_holder._ancestor_vtx.z(); }
    inline double       ancestor_t        () const { return _particle_holder._ancestor_vtx.t(); }

    //
    // Setters
    //
    // generator/origin info setter
    inline void id              (InstanceID_t id  )  { _particle_holder._id = id;         }
    inline void mcst_index      (MCSTIndex_t id )    { _particle_holder._mcst_index = id;    }
    inline void mct_index       (MCTIndex_t id )     { _particle_holder._mct_index = id;     }
    inline void shape           (ShapeType_t shape ) { _particle_holder._shape = shape;      }
    inline void nu_current_type (short curr) {_particle_holder._current_type = curr; }
    inline void nu_interaction_type (short itype) {_particle_holder._interaction_type = itype; }
    // particle's info setter
    inline void track_id        (unsigned int id )   { _particle_holder._trackid = id;       }
    inline void pdg_code        (int code)           { _particle_holder._pdg = code;         }
    inline void momentum        (double px, double py, double pz) { 
      _particle_holder._px = px; _particle_holder._py = py; _particle_holder._pz = pz; }
    inline void position        (const larcv3::Vertex& vtx) { _particle_holder._vtx = vtx;    }
    inline void position        (double x, double y, double z, double t) { _particle_holder._vtx = Vertex(x,y,z,t); }
    inline void end_position    (const larcv3::Vertex& vtx) { _particle_holder._end_pt = vtx; }
    inline void end_position    (double x, double y, double z, double t) { _particle_holder._end_pt = Vertex(x,y,z,t); }
    inline void first_step      (const larcv3::Vertex& vtx) { _particle_holder._first_step = vtx; }
    inline void first_step      (double x, double y, double z, double t) { _particle_holder._first_step = Vertex(x,y,z,t); }
    inline void last_step       (const larcv3::Vertex& vtx) { _particle_holder._last_step = vtx; }
    inline void last_step       (double x, double y, double z, double t) { _particle_holder._last_step = Vertex(x,y,z,t); }
    inline void distance_travel ( double dist ) { _particle_holder._dist_travel = dist; }
    inline void energy_init     (double e)           { _particle_holder._energy_init = e;    }
    inline void energy_deposit  (double e)           { _particle_holder._energy_deposit = e; }
           void creation_process (const std::string& proc);
    // inline void boundingbox_2d(const std::vector<larcv3::BBox2D>& bb_v) { _bb2d_v = bb_v; }
    // inline void boundingbox_2d(const BBox2D& bb, ProjectionID_t id) { _bb2d_v.resize(id+1); _bb2d_v[id] = bb; }
    // inline void boundingbox_3d(const BBox3D& bb) { _bb3d = bb; }
    // inline void num_voxels(int count) { _num_voxels = count; }
    //inline void type_score (const std::vector<float>& score_v) { _type_score_v = score_v; }
    // parent info setter
    inline void parent_track_id (unsigned int id )   { _particle_holder._parent_trackid = id;}
    inline void parent_pdg_code (int code)           { _particle_holder._parent_pdg = code;  }
    inline void parent_position (const larcv3::Vertex& vtx) { _particle_holder._parent_vtx = vtx; }
    inline void parent_position (double x, double y, double z, double t) { _particle_holder._parent_vtx = Vertex(x,y,z,t); }
    // ancestor info setter
    inline void ancestor_track_id (unsigned int id )   { _particle_holder._ancestor_trackid = id;}
    inline void ancestor_pdg_code (int code)           { _particle_holder._ancestor_pdg = code;  }
    inline void ancestor_position (const larcv3::Vertex& vtx) { _particle_holder._ancestor_vtx = vtx; }
    inline void ancestor_position (double x, double y, double z, double t) { _particle_holder._ancestor_vtx = Vertex(x,y,z,t); }

    std::string dump() const;

    // These functions only appear in larcv proper, not in included headers:
#ifdef LARCV_INTERNAL
    // Return a numpy array of this object (no copy by default)
    pybind11::array_t<ParticleHolder> as_array();
#endif


  public:
    static hid_t get_datatype() {
      hid_t datatype;
      datatype = H5Tcreate (H5T_COMPOUND, sizeof (ParticleHolder));

      // Get the compound types:
      hid_t string_type = H5Tcopy (H5T_C_S1);
      H5Tset_size(string_type, PARTICLE_PROCESS_STRLEN);

      H5Tinsert(datatype, "id",
                HOFFSET(Particle, _particle_holder._id),
                larcv3::get_datatype<InstanceID_t>());
      H5Tinsert(datatype, "mcst_index",
                HOFFSET(Particle, _particle_holder._mcst_index),
                larcv3::get_datatype<MCSTIndex_t>());
      H5Tinsert(datatype, "mct_index",
                HOFFSET(Particle, _particle_holder._mct_index),
                larcv3::get_datatype<MCTIndex_t>());
      H5Tinsert(datatype, "shape",
                HOFFSET(Particle, _particle_holder._shape),
                larcv3::get_datatype<ShapeType_t>());
      H5Tinsert(datatype, "current_type",
                HOFFSET(Particle, _particle_holder._current_type),
                larcv3::get_datatype<short>());
      H5Tinsert(datatype, "interaction_type",
                HOFFSET(Particle, _particle_holder._interaction_type),
                larcv3::get_datatype<short>());
      H5Tinsert(datatype, "trackid",
                HOFFSET(Particle, _particle_holder._trackid),
                larcv3::get_datatype<unsigned int>());
      H5Tinsert(datatype, "pdg",
                HOFFSET(Particle, _particle_holder._pdg),
                larcv3::get_datatype<int>());
      H5Tinsert(datatype, "px",
                HOFFSET(Particle, _particle_holder._px),
                larcv3::get_datatype<double>());
      H5Tinsert(datatype, "py",
                HOFFSET(Particle, _particle_holder._py),
                larcv3::get_datatype<double>());
      H5Tinsert(datatype, "pz",
                HOFFSET(Particle, _particle_holder._pz),
                larcv3::get_datatype<double>());
      H5Tinsert(datatype, "vtx",
                HOFFSET(Particle, _particle_holder._vtx),
                Vertex::get_datatype());
      H5Tinsert(datatype, "end_pt",
                HOFFSET(Particle, _particle_holder._end_pt),
                Vertex::get_datatype());
      H5Tinsert(datatype, "first_step",
                HOFFSET(Particle, _particle_holder._first_step),
                Vertex::get_datatype());
      H5Tinsert(datatype, "last_step",
                HOFFSET(Particle, _particle_holder._last_step),
                Vertex::get_datatype());
      H5Tinsert(datatype, "dist_travel",
                HOFFSET(Particle, _particle_holder._dist_travel),
                larcv3::get_datatype<double>());
      H5Tinsert(datatype, "energy_init",
                HOFFSET(Particle, _particle_holder._energy_init),
                larcv3::get_datatype<double>());
      H5Tinsert(datatype, "energy_deposit",
                HOFFSET(Particle, _particle_holder._energy_deposit),
                larcv3::get_datatype<double>());
      H5Tinsert(datatype, "process",
                HOFFSET(Particle, _particle_holder._process),
                string_type);
      H5Tinsert(datatype, "parent_trackid",
                HOFFSET(Particle, _particle_holder._parent_trackid),
                larcv3::get_datatype<unsigned int>());
      H5Tinsert(datatype, "parent_pdg",
                HOFFSET(Particle, _particle_holder._parent_pdg),
                larcv3::get_datatype<int>());
      H5Tinsert(datatype, "parent_vtx",
                HOFFSET(Particle, _particle_holder._parent_vtx),
                Vertex::get_datatype());
      H5Tinsert(datatype, "ancestor_trackid",
                HOFFSET(Particle, _particle_holder._ancestor_trackid),
                larcv3::get_datatype<unsigned int>());
      H5Tinsert(datatype, "ancestor_pdg",
                HOFFSET(Particle, _particle_holder._ancestor_pdg),
                larcv3::get_datatype<int>());
      H5Tinsert(datatype, "ancestor_vtx",
                HOFFSET(Particle, _particle_holder._ancestor_vtx),
                Vertex::get_datatype());

      return datatype;
    }

  public:

    ParticleHolder _particle_holder;
    

  };


}
#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_particle(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group
