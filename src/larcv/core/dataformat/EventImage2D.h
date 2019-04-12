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
#ifndef EVENTIMAGE_H
#define EVENTIMAGE_H

#include <iostream>
#include "EventBase.h"
#include "Image2D.h"
#include "DataProductFactory.h"

namespace larcv {
  
  /**
    \class EventImage2D
    Event-wise class to store a collection of larcv::Image
  */
  class EventImage2D : public EventBase {
    
  public:
    
    /// Clears an array of larcv::Image
    void clear();

    /// Const reference getter to an array of larcv::Image
    const std::vector<larcv::Image2D>& as_vector() const { return _image_v; }

    /// Deprecated (use as_vector): const reference getter to an array of larcv::Image2D 
    const std::vector<larcv::Image2D>& image2d_array() const { return _image_v; }

    /// larcv::Image2D const reference getter for a specified index number
    const Image2D& at(ImageIndex_t id) const;

    /// Inserter into larcv::Image2D array
    void append(const Image2D& img);
    /// Emplace into larcv::Image2D array
    void emplace(Image2D&& img);
    /// Emplace into larcv::Image2D array
    void emplace(std::vector<larcv::Image2D>&& image_v);
    /// std::move to retrieve content larcv::Image2D array
    void move(std::vector<larcv::Image2D>& image_v)
    { image_v = std::move(_image_v); }
    
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);

    static EventImage2D * to_image2d(EventBase * e){
      return (EventImage2D *) e;
    }
  private:

    std::vector<larcv::Image2D> _image_v;

  };

}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventImage2D>() { return "image2d"; }
  /**
     \class larcv::EventImage2D
     \brief A concrete factory class for larcv::EventImage2D
  */
  class EventImage2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventImage2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventImage2D>(),this); }
    /// create method
    EventBase* create() { return new EventImage2D; }
  };

}

#endif
/** @} */ // end of doxygen group 

