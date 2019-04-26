/*
 * \file EventVoxel2D.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv3::EventSparseTensor and larcv3::EventClusterPixel2D
 *
 * @author kazuhiro
 * @author cadams
 

 * \addtogroup core_DataFormat

*/
#ifndef __LARCV3_EVENTVOXEL_H
#define __LARCV3_EVENTVOXEL_H

#include <iostream>
#include "EventBase.h"
#include "DataProductFactory.h"
#include "Voxel.h"
#include "ImageMeta.h"
#include "VoxelSerializationHelper.h"

namespace larcv3 {

  /**
    \class EventSparseCluster2D
    \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  */
  template<size_t dimension>
  class EventSparseCluster : public EventBase {

  public:

    /// Default constructor
    EventSparseCluster() {}

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
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);

    static EventSparseCluster * to_sparse_cluster(EventBase * e){
      return (EventSparseCluster *) e;
    }

  private:
    std::vector<larcv3::SparseCluster<dimension> > _cluster_v;
    VoxelSerializationHelper<dimension> _helper;
    
  };

  /**
    \class EventSparseTensor
    \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  */
  template<size_t dimension> 
  class EventSparseTensor : public EventBase {

  public:

    /// Default constructor
    EventSparseTensor() {}


    //
    // Read-access
    //
    /// Access to all stores larcv3::SparseTensor
    inline const std::vector<larcv3::SparseTensor<dimension> >& as_vector() const { return _tensor_v; }
    /// Access SparseTensor<dimension>  of a specific projection ID
    const larcv3::SparseTensor<dimension> & sparse_tensor(const ProjectionID_t id) const;
    /// Number of valid projection id
    inline size_t size() const { return _tensor_v.size(); }

    //
    // Write-access
    //
    /// EventBase::clear() override
    void clear() {}
    /// Emplace data
    void emplace(larcv3::SparseTensor<dimension> && voxels);
    /// Set data`
    void set(const larcv3::SparseTensor<dimension> & voxels);
    /// Emplace a new element
    void emplace(larcv3::VoxelSet&& voxels, larcv3::ImageMeta<dimension>&& meta);
    /// Set a new element
    void set(const larcv3::VoxelSet& voxels, const larcv3::ImageMeta<dimension>& meta);


    // IO functions:
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);


    static EventSparseTensor * to_sparse_tensor(EventBase * e){
      return (EventSparseTensor *) e;
    }

  private:

    std::vector<larcv3::SparseTensor<dimension> >  _tensor_v;
    VoxelSerializationHelper<dimension> _helper;
    
  };


typedef EventSparseTensor<2>  EventSparseTensor2D;
typedef EventSparseTensor<3>  EventSparseTensor3D;

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

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::EventSparseTensor2D>() { return "sparse2d"; }
  template<> inline std::string product_unique_name<larcv3::EventSparseTensor3D>() { return "sparse3d"; }

  /**
     \class larcv3::EventSparseTensor
     \brief A concrete factory class for larcv3::EventSparseTensor
  */



  class EventSparseTensor2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensor2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventSparseTensor2D>(), this); }
    /// dtor
    ~EventSparseTensor2DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor2D; }
  };

  class EventSparseTensor3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensor3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventSparseTensor3D>(), this); }
    /// dtor
    ~EventSparseTensor3DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor3D; }
  };

}

#endif
/** @} */ // end of doxygen group

