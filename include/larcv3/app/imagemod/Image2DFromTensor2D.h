/**
 * \file Image2DFromTensor2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class Image2DFromTensor2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV_IMAGE2DFROMTENSOR2D_H__
#define __LARCV_IMAGE2DFROMTENSOR2D_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class Image2DFromTensor2D ... these comments are used to generate
     doxygen documentation!
  */
  class Image2DFromTensor2D : public ProcessBase {

  public:
    
    /// Default constructor
    Image2DFromTensor2D(const std::string name="Image2DFromTensor2D");
    
    /// Default destructor
    ~Image2DFromTensor2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    enum PIType_t {
      kPITypeFixedPI,
      kPITypeInputVoxel,
      kPITypeUndefined
    };
    std::vector<float> _base_pi_v;
    std::vector<float> _fixed_pi_v;
    std::vector<unsigned short> _type_pi_v;
    std::vector<std::string> _tensor2d_producer_v;
    std::vector<std::string> _output_producer_v;

  };

  /**
     \class larcv3::Image2DFromTensor2DFactory
     \brief A concrete factory class for larcv3::Image2DFromTensor2D
  */
  class Image2DFromTensor2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Image2DFromTensor2DProcessFactory() { ProcessFactory::get().add_factory("Image2DFromTensor2D",this); }
    /// dtor
    ~Image2DFromTensor2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Image2DFromTensor2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

