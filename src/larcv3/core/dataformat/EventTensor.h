/**
 * \file EventTensor.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventTensor (was EventImage2D)
 *
 * @author kazuhiro
 * @author cadams
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_EVENTTENSOR_H
#define __LARCV3DATAFORMAT_EVENTTENSOR_H

#include <iostream>
#include "larcv3/core/dataformat/EventBase.h"
#include "larcv3/core/dataformat/Tensor.h"
#include "larcv3/core/dataformat/DataProductFactory.h"

namespace larcv3 {
  
  /**
    \class EventTensor
    Event-wise class to store a collection of larcv3::Tensor
  */
  template<size_t dimension>
  class EventTensor : public EventBase {
    
  public:
    
    EventTensor();

    /// Const reference getter to an array of larcv3::Tensor<dimension>
    const std::vector<larcv3::Tensor<dimension>>& as_vector() const { return _image_v; }

    /// Deprecated (use as_vector): const reference getter to an array of larcv3::Tensor<dimension> 
    const std::vector<larcv3::Tensor<dimension>>& image2d_array() const { return _image_v; }

    /// Clears an array of larcv3::Tensor<dimension>
    void clear();
    /// Inserter into larcv3::Tensor<dimension> array
    void append(const Tensor<dimension>& img);
    /// Emplace into larcv3::Tensor<dimension> array
    void emplace(Tensor<dimension>&& img);
    /// Emplace into larcv3::Tensor<dimension> array
    void emplace(std::vector<larcv3::Tensor<dimension>>&& image_v);
    /// std::move to retrieve content larcv3::Tensor<dimension> array
    void move(std::vector<larcv3::Tensor<dimension>>& image_v)
    { image_v = std::move(_image_v); }
    
    void initialize (hid_t group, uint compression);
    void serialize  (hid_t group);
    void deserialize(hid_t group, size_t entry, bool reopen_groups=false);
    void finalize   ();
    
    /// For backward compatibility
    static EventTensor * to_image2d(EventBase * e){
      return to_tensor(e);
    }

    static EventTensor * to_tensor(EventBase * e){
      return (EventTensor *) e;
    }
    
  private:
    void open_in_datasets(hid_t group);
    void open_out_datasets(hid_t group);

    std::vector<larcv3::Tensor<dimension>> _image_v;

    uint _compression;

  };

  typedef EventTensor<1>  EventTensor1D;
  typedef EventTensor<2>  EventImage2D;
  typedef EventTensor<3>  EventTensor3D;
  typedef EventTensor<4>  EventTensor4D;

}

#include "IOManager.h"
namespace larcv3 {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::EventTensor1D>() { return "tensor1d"; }
  template<> inline std::string product_unique_name<larcv3::EventImage2D>()  { return "image2d";  }
  template<> inline std::string product_unique_name<larcv3::EventTensor3D>() { return "tensor3d"; }
  template<> inline std::string product_unique_name<larcv3::EventTensor4D>() { return "tensor4d"; }


  /**
     \class larcv3::EventTensor
     \brief A concrete factory class for larcv3::EventTensor
  */

  template<size_t dimension>
  class EventTensorFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventTensorFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventTensor<dimension>>(),this); }
    /// dtor
    ~EventTensorFactory() {}
    /// create method
    EventBase* create() { return new EventTensor<dimension>; }
  };

}

#endif
/** @} */ // end of doxygen group 

