
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

#ifndef __LARCV3DATAFORMAT_EVENTSPARSETENSOR_H
#define __LARCV3DATAFORMAT_EVENTSPARSETENSOR_H


#include <iostream>
#include "larcv3/core/dataformat/EventBase.h"
#include "larcv3/core/dataformat/DataProductFactory.h"
#include "larcv3/core/dataformat/Voxel.h"
#include "larcv3/core/dataformat/ImageMeta.h"



namespace larcv3 {

  /**
    \class EventSparseTensor
    \brief Event-wise class to store a collection of VoxelSet (cluster) per projection id
  */
  template<size_t dimension> 
  class EventSparseTensor : public EventBase {

  public:

    /// Default constructor
    EventSparseTensor();


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
    void initialize (hid_t group, uint compression);
    void serialize  (hid_t group);
    void deserialize(hid_t group, size_t entry, bool reopen_groups=false);
    void finalize   ();

    static EventSparseTensor * to_sparse_tensor(EventBase * e){
      return (EventSparseTensor *) e;
    }

  private:
    void open_in_datasets(hid_t group);
    void open_out_datasets(hid_t group);
    
    std::vector<larcv3::SparseTensor<dimension> >  _tensor_v;



  };


typedef EventSparseTensor<2>  EventSparseTensor2D;
typedef EventSparseTensor<3>  EventSparseTensor3D;


}


#include "IOManager.h"
namespace larcv3 {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::EventSparseTensor2D>() { return "sparse2d"; }
  template<> inline std::string product_unique_name<larcv3::EventSparseTensor3D>() { return "sparse3d"; }


  /**
     \class larcv3::EventSparseTensorFactory
     \brief A concrete factory class for larcv3::EventSparseTensor
  */


  template<size_t dimension>
  class EventSparseTensorFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensorFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventSparseTensor<dimension>>(), this); }
    /// dtor
    ~EventSparseTensorFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor<dimension>; }
  };

}

#endif
/** @} */ // end of doxygen group
