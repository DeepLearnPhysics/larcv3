/**
 * \file EventBBox.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class EventBBox
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_EVENTBBOX_H
#define __LARCV3DATAFORMAT_EVENTBBOX_H

#include "larcv3/core/dataformat/EventBase.h"
#include "larcv3/core/dataformat/BBox.h"
#include "larcv3/core/dataformat/DataProductFactory.h"

class EventBBox;

namespace larcv3 {
  /**
    \class EventBBox
    User-defined data product class (please comment!)
  */
  template<size_t dimension>
  class EventBBox : public EventBase {

  public:

    /// Default constructor
    EventBBox();

    /// Default destructor
    ~EventBBox(){}

    inline larcv3::BBoxCollection<dimension> at(size_t index) {return _bbox_c_v.at(index);}



    void set(const std::vector<larcv3::BBoxCollection<dimension>>& bbox_c_v);
    void append(const larcv3::BBoxCollection<dimension>& bbox_c);
    void emplace_back(larcv3::BBoxCollection<dimension>&& bbox_c);
    void emplace(std::vector<larcv3::BBoxCollection<dimension>>&& bbox_c_v);

    inline const std::vector<larcv3::BBoxCollection<dimension>>& as_vector() const
    { return _bbox_c_v; }

    // inline const larcv3::BBox<dimension>& at(size_t index) const {return _bbox_c_v.at(index);}

    inline size_t size() const {return _bbox_c_v.size();}

    /// Data clear method
    void clear      ();
    void initialize (hid_t group, uint compression);
    void serialize  (hid_t group);
    void deserialize(hid_t group, size_t entry, bool reopen_groups=false);
    void finalize   ();


  private:

    void open_in_datasets(hid_t group);
    void open_out_datasets(hid_t group);

    std::vector<larcv3::BBoxCollection<dimension>> _bbox_c_v; ///< a collection of particles (index maintained)

  };

  typedef EventBBox<2> EventBBox2D;
  typedef EventBBox<3> EventBBox3D;
}


#include "IOManager.h"
namespace larcv3 {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv3::EventBBox2D>() { return "bbox2d"; }
  template<> inline std::string product_unique_name<larcv3::EventBBox3D>() { return "bbox3d"; }


  /**
     \class larcv3::EventTensor
     \brief A concrete factory class for larcv3::EventTensor
  */

  template<size_t dimension>
  class EventBBoxFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventBBoxFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv3::EventBBox<dimension>>(),this); }
    /// dtor
    ~EventBBoxFactory() {}
    /// create method
    EventBase* create() { return new EventBBox<dimension>; }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>

template <size_t dimension>
void init_eventbbox_instance(pybind11::module m);

void init_eventbbox(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group
