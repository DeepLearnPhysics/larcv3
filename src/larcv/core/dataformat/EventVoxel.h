/**
 * \file EventVoxel2D.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::EventSparseTensor and larcv::EventClusterPixel2D
 *
 * @author kazuhiro
 * @author cadams
 */

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

  /**
    \class EventSparseClusters
    \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  */
  class EventSparseClusters : public EventBase {

  public:

    /// Default constructor
    EventSparseClusters() {}

    /// Default destructor
    virtual ~EventSparseClusters() {}

    /// EventBase::clear() override
    inline void clear() {_cluster_v.clear();}

    /// Access to all stores larcv::SparseCluster
    inline const std::vector<larcv::SparseCluster>& as_vector() const { return _cluster_v; }

    /// Access SparseCluster of a specific projection ID
    const larcv::SparseCluster& sparse_cluster(const ProjectionID_t id) const;

    /// Number of valid projection id
    inline size_t size() const { return _cluster_v.size(); }

    //
    // Write-access
    //
    /// Emplace data
    void emplace(larcv::SparseCluster&& clusters);
    /// Set data
    void set(const larcv::SparseCluster& clusters);
    /// Emplace a new element
    void emplace(larcv::VoxelSetArray&& clusters, larcv::ImageMeta&& meta);
    // /// Set a new element
    // void set(const larcv::VoxelSetArray& clusters, const larcv::ImageMeta& meta);


    // IO functions:
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);


  private:
    std::vector<larcv::SparseCluster> _cluster_v;
    VoxelSerializationHelper _helper;
    
  };

  /**
    \class EventSparseTensor
    \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  */
  class EventSparseTensor : public EventBase {
    friend VoxelSerializationHelper;
  public:

    /// Default constructor
    EventSparseTensor() {}


    //
    // Read-access
    //
    /// Access to all stores larcv::SparseTensor
    inline const std::vector<larcv::SparseTensor>& as_vector() const { return _tensor_v; }
    /// Access SparseTensor of a specific projection ID
    const larcv::SparseTensor& sparse_tensor(const ProjectionID_t id) const;
    /// Number of valid projection id
    inline size_t size() const { return _tensor_v.size(); }

    //
    // Write-access
    //
    /// EventBase::clear() override
    void clear() {_tensor_v.clear();}
    /// Emplace data
    void emplace(larcv::SparseTensor&& voxels);
    /// Set data`
    void set(const larcv::SparseTensor& voxels);
    /// Emplace a new element
    void emplace(larcv::VoxelSet&& voxels, larcv::ImageMeta&& meta);
    /// Set a new element
    void set(const larcv::VoxelSet& voxels, const larcv::ImageMeta& meta);


    // IO functions:
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);


    static EventSparseTensor * to_sparse_tensor(EventBase * e){
      return (EventSparseTensor *) e;
    }

  private:

    std::vector<larcv::SparseTensor>  _tensor_v;
    VoxelSerializationHelper _helper;
    
  };

}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventSparseClusters>() { return "cluster"; }
  // template EventSparseClusters& IOManager::get_data<larcv::EventSparseClusters>(const std::string&);
  // template EventSparseClusters& IOManager::get_data<larcv::EventSparseClusters>(const ProducerID_t);

  /**
  //    \class larcv::EventSparseClusters
  //    \brief A concrete factory class for larcv::EventSparseClusters
  */

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

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventSparseTensor>() { return "sparse"; }
  // template EventSparseTensor& IOManager::get_data<larcv::EventSparseTensor>(const std::string&);
  // template EventSparseTensor& IOManager::get_data<larcv::EventSparseTensor>(const ProducerID_t);

  /**
     \class larcv::EventSparseTensor
     \brief A concrete factory class for larcv::EventSparseTensor
  */
  class EventSparseTensorFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensorFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventSparseTensor>(), this); }
    /// dtor
    ~EventSparseTensorFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor; }
  };


}

#endif
/** @} */ // end of doxygen group

