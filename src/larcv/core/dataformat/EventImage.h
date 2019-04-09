/**
 * \file EventImage.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventImage
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
#include "Image.h"
#include "DataProductFactory.h"

namespace larcv {
  
  /**
    \class EventImage
    Event-wise class to store a collection of larcv::Image
  */
  class EventImage : public EventBase {
    
  public:
    
    /// Clears an array of larcv::Image
    void clear();

    /// Const reference getter to an array of larcv::Image
    const std::vector<larcv::Image>& as_vector() const { return _image_v; }

    /// Deprecated (use as_vector): const reference getter to an array of larcv::Image 
    const std::vector<larcv::Image>& image_array() const { return _image_v; }

    /// larcv::Image const reference getter for a specified index number
    const Image& at(ImageIndex_t id) const;

    /// Inserter into larcv::Image array
    void append(const Image& img);
    /// Emplace into larcv::Image array
    void emplace(Image&& img);
    /// Emplace into larcv::Image array
    void emplace(std::vector<larcv::Image>&& image_v);
    /// std::move to retrieve content larcv::Image array
    void move(std::vector<larcv::Image>& image_v)
    { image_v = std::move(_image_v); }
    
    void initialize (H5::Group * group);
    void serialize  (H5::Group * group);
    void deserialize(H5::Group * group, size_t entry);

  private:

    std::vector<larcv::Image> _image_v;

  };

}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventImage>() { return "image"; }
  /**
     \class larcv::EventImage
     \brief A concrete factory class for larcv::EventImage
  */
  class EventImageFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventImageFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventImage>(),this); }
    /// create method
    EventBase* create() { return new EventImage; }
  };

}

#endif
/** @} */ // end of doxygen group 

