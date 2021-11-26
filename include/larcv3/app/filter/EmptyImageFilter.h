/**
 * \file EmptyImageFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EmptyImageFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV3FILTER_EMPTYIMAGEFILTER_H__
#define __LARCV3FILTER_EMPTYIMAGEFILTER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class EmptyImageFilter ... these comments are used to generate
     doxygen documentation!
  */
  class EmptyImageFilter : public larcv3::ProcessBase {

  public:
    
    /// Default constructor
    EmptyImageFilter(const std::string name="EmptyImageFilter");
    
    /// Default destructor
    ~EmptyImageFilter(){}

    void configure(const larcv3::PSet&);

    void initialize();

    bool process(larcv3::IOManager& mgr);

    void finalize();

  private:
    std::string _image_producer;

  };

  /**
     \class larcv3::EmptyImageFilterFactory
     \brief A concrete factory class for larcv3::EmptyImageFilter
  */
  class EmptyImageFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EmptyImageFilterProcessFactory() { ProcessFactory::get().add_factory("EmptyImageFilter",this); }
    /// dtor
    ~EmptyImageFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EmptyImageFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

