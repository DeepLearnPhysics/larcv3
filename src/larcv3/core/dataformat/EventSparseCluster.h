/*
 * \file EventSparseCluster.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv3::EventSparseCluster 2D and 3D
 *
 * @author kazuhiro
 * @author cadams
 

 * \addtogroup core_DataFormat

*/
#ifndef __LARCV3DATAFORMAT_EVENTSPARSECLUSTER_H
#define __LARCV3DATAFORMAT_EVENTSPARSECLUSTER_H

#include <iostream>
#include "larcv3/core/dataformat/EventBase.h"
#include "larcv3/core/dataformat/DataProductFactory.h"
#include "larcv3/core/dataformat/Voxel.h"
#include "larcv3/core/dataformat/ImageMeta.h"

namespace larcv3 {

  /**
    \class EventSparseCluster2D
    \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  */
  template<size_t dimension>
  class EventSparseCluster : public EventBase {

  public:

    /// Default constructor
    EventSparseCluster();

    /// Default destructor
    virtual ~EventSparseCluster() {}

    /// EventBase::clear() override
    inline void clear() {_cluster_v.clear();}

    /// Access to all stores larcv3::SparseCluster
    inline const std::vector<larcv3::SparseCluster<dimension> >& as_vector() const { return _cluster_v; }

    /// Access SparseCluster of a specific projection ID
    const larcv3::SparseCluster<dimension> & sparse_cluster(const ProjectionID_t id) const;

    /// Number of valid projection id
    inline size_t size() const { return _cluster_v.size(); }

    //
    // Write-access
    //
    /// Emplace data
    void emplace(larcv3::SparseCluster<dimension>&& clusters);
    /// Set data
    void set(const larcv3::SparseCluster<dimension>& clusters);
    /// Emplace a new element
    void emplace(larcv3::VoxelSetArray&& clusters, larcv3::ImageMeta<dimension>&& meta);
    // /// Set a new element
    // void set(const larcv3::VoxelSetArray& clusters, const larcv3::ImageMeta& meta);



    // IO functions:
    void initialize (hid_t group, uint compression);
    void serialize  (hid_t group);
    void deserialize(hid_t group, size_t entry, bool reopen_groups=false);
    void finalize   ();


    static EventSparseCluster * to_sparse_cluster(EventBase * e){
      return (EventSparseCluster *) e;
    }

  private:
    void open_in_datasets(hid_t group);
    void open_out_datasets(hid_t group);
    std::vector<larcv3::SparseCluster<dimension> > _cluster_v;

  };

typedef EventSparseCluster<2> EventSparseCluster2D;
typedef EventSparseCluster<3> EventSparseCluster3D;

}


#include "IOManager.h"
namespace larcv3 {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::EventSparseCluster2D>() { return "cluster2d"; }
  template<> inline std::string product_unique_name<larcv3::EventSparseCluster3D>() { return "cluster3d"; }

  
  //    \class larcv3::EventSparseCluster
  //    \brief A concrete factory class for larcv3::EventSparseCluster
  

  class EventSparseCluster2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseCluster2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventSparseCluster2D>(), this); }
    /// dtor
    ~EventSparseCluster2DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseCluster2D; }
  };
  
  class EventSparseCluster3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseCluster3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventSparseCluster3D>(), this); }
    /// dtor
    ~EventSparseCluster3DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseCluster3D; }
  };

}

#endif
/** @} */ // end of doxygen group

