/**
 * \file EventImage2D.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventImage2D
 *
 * @author kazuhiro
 * @author cadams
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_EVENTIMAGE_H
#define __LARCV3DATAFORMAT_EVENTIMAGE_H

#include <iostream>
#include "larcv3/core/dataformat/EventBase.h"
#include "larcv3/core/dataformat/Image2D.h"
#include "larcv3/core/dataformat/DataProductFactory.h"

namespace larcv3 {
  
  /**
    \class EventImage2D
    Event-wise class to store a collection of larcv3::Image
  */
  class EventImage2D : public EventBase {
    
  public:
    
    EventImage2D();

    /// Clears an array of larcv3::Image
    void clear();

    /// Const reference getter to an array of larcv3::Image
    const std::vector<larcv3::Image2D>& as_vector() const { return _image_v; }

    /// Deprecated (use as_vector): const reference getter to an array of larcv3::Image2D 
    const std::vector<larcv3::Image2D>& image2d_array() const { return _image_v; }


    /// Inserter into larcv3::Image2D array
    void append(const Image2D& img);
    /// Emplace into larcv3::Image2D array
    void emplace(Image2D&& img);
    /// Emplace into larcv3::Image2D array
    void emplace(std::vector<larcv3::Image2D>&& image_v);
    /// std::move to retrieve content larcv3::Image2D array
    void move(std::vector<larcv3::Image2D>& image_v)
    { image_v = std::move(_image_v); }
    
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry, bool reopen_groups=false);

    static EventImage2D * to_image2d(EventBase * e){
      return (EventImage2D *) e;
    }
  private:
    void open_in_datasets(H5::Group * group);
    void open_out_datasets(H5::Group * group);

    std::vector<larcv3::Image2D> _image_v;


  };

}

#include "IOManager.h"
namespace larcv3 {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::EventImage2D>() { return "image2d"; }
  /**
     \class larcv3::EventImage2D
     \brief A concrete factory class for larcv3::EventImage2D
  */
  class EventImage2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventImage2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventImage2D>(),this); }
    /// create method
    EventBase* create() { return new EventImage2D; }
  };

}

#endif
/** @} */ // end of doxygen group 

