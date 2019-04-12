/*
 * \file EventVoxel2D.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::EventSparseTensor and larcv::EventClusterPixel2D
 *
 * @author kazuhiro
 * @author cadams
 

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV_EVENTVOXEL_H
#define __LARCV_EVENTVOXEL_H

#include <iostream>
#include "EventBase.h"
#include "DataProductFactory.h"
#include "Voxel.h"
#include "ImageMeta.h"
#include "VoxelSerializationHelper.h"

namespace larcv {

  // /**
  //   \class EventSparseClusters2D
  //   \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  // */
  // template<size_t dimension>
  // class EventSparseCluster : public EventBase {

  // public:

  //   /// Default constructor
  //   EventSparseCluster() {}

  //   /// Default destructor
  //   virtual ~EventSparseCluster() {}

  //   /// EventBase::clear() override
  //   inline void clear() {_cluster_v.clear();}

  //   /// Access to all stores larcv::SparseCluster
  //   inline const std::vector<larcv::SparseCluster<dimension> >& as_vector() const { return _cluster_v; }

  //   /// Access SparseCluster of a specific projection ID
  //   const larcv::SparseCluster<dimension> & sparse_cluster(const ProjectionID_t id) const;

  //   /// Number of valid projection id
  //   inline size_t size() const { return _cluster_v.size(); }

  //   //
  //   // Write-access
  //   //
  //   /// Emplace data
  //   void emplace(larcv::SparseCluster&& clusters);
  //   /// Set data
  //   void set(const larcv::SparseCluster& clusters);
  //   /// Emplace a new element
  //   void emplace(larcv::VoxelSetArray&& clusters, larcv::ImageMeta<dimension>&& meta);
  //   // /// Set a new element
  //   // void set(const larcv::VoxelSetArray& clusters, const larcv::ImageMeta& meta);



  //   // IO functions:
  //   void initialize (H5::Group * group);
  //   void serialize  (H5::Group * group);
  //   void deserialize(H5::Group * group, size_t entry);

  //   static EventSparseCluster * to_sparse_cluster2d(EventBase * e){
  //     return (EventSparseCluster *) e;
  //   }

  // private:
  //   std::vector<larcv::SparseCluster<dimension> > _cluster_v;
  //   VoxelSerializationHelper _helper;
    
  // };

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
    /// Access to all stores larcv::SparseTensor
    inline const std::vector<larcv::SparseTensor<dimension> >& as_vector() const { return _tensor_v; }
    /// Access SparseTensor<dimension>  of a specific projection ID
    const larcv::SparseTensor<dimension> & sparse_tensor(const ProjectionID_t id) const;
    /// Number of valid projection id
    inline size_t size() const { return _tensor_v.size(); }

    //
    // Write-access
    //
    /// EventBase::clear() override
    void clear() {}
    /// Emplace data
    void emplace(larcv::SparseTensor<dimension> && voxels);
    /// Set data`
    void set(const larcv::SparseTensor<dimension> & voxels);
    /// Emplace a new element
    void emplace(larcv::VoxelSet&& voxels, larcv::ImageMeta<dimension>&& meta);
    /// Set a new element
    void set(const larcv::VoxelSet& voxels, const larcv::ImageMeta<dimension>& meta);


    // IO functions:
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);


    static EventSparseTensor * to_sparse_tensor(EventBase * e){
      return (EventSparseTensor *) e;
    }

  private:

    std::vector<larcv::SparseTensor<dimension> >  _tensor_v;
    VoxelSerializationHelper<dimension> _helper;
    
  };


typedef EventSparseTensor<2> EventSparseTensor2D;
typedef EventSparseTensor<3> EventSparseTensor3D;


}


#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  // template<> inline std::string product_unique_name<larcv::EventSparseClusters>() { return "cluster"; }
  // template EventSparseClusters& IOManager::get_data<larcv::EventSparseClusters>(const std::string&);
  // template EventSparseClusters& IOManager::get_data<larcv::EventSparseClusters>(const ProducerID_t);

  /*
  //    \class larcv::EventSparseClusters
  //    \brief A concrete factory class for larcv::EventSparseClusters
  

  class EventSparseClustersFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseClustersFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventSparseClusters>(), this); }
    /// dtor
    ~EventSparseClustersFactory() {}
    /// create method
    EventBase* create() { return new EventSparseClusters; }
  };
  */

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventSparseTensor2D>() { return "sparse2d"; }
  template<> inline std::string product_unique_name<larcv::EventSparseTensor3D>() { return "sparse3d"; }
  // template EventSparseTensor& IOManager::get_data<larcv::EventSparseTensor>(const std::string&);
  // template EventSparseTensor& IOManager::get_data<larcv::EventSparseTensor>(const ProducerID_t);

  /**
     \class larcv::EventSparseTensor
     \brief A concrete factory class for larcv::EventSparseTensor
  */



  class EventSparseTensor2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensor2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventSparseTensor2D>(), this); }
    /// dtor
    ~EventSparseTensor2DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor2D; }
  };

  class EventSparseTensor3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensor3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventSparseTensor3D>(), this); }
    /// dtor
    ~EventSparseTensor3DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor3D; }
  };

}

#endif
/** @} */ // end of doxygen group

