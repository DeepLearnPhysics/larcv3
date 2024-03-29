/**
 * \file BatchFillerPIDLabel.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerPIDLabel
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERPIDLABEL_H__
#define __LARCV3THREADIO_BATCHFILLERPIDLABEL_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerPIDLabel ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerPIDLabel : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerPIDLabel(const std::string name="BatchFillerPIDLabel");
    
    /// Default destructor
    ~BatchFillerPIDLabel(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void _batch_begin_();

    void _batch_end_();

    void finalize();

    static json default_config(){
      json c = {
        {"Producer", std::string()},
        {"PdgClassList", std::vector<int>()},
      };
      return c;
    }

  private:

    json config;

    std::vector<float> _entry_data;
  };

  /**
     \class larcv3::BatchFillerPIDLabelFactory
     \brief A concrete factory class for larcv3::BatchFillerPIDLabel
  */
  class BatchFillerPIDLabelProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerPIDLabelProcessFactory() { ProcessFactory::get().add_factory("BatchFillerPIDLabel",this); }
    /// dtor
    ~BatchFillerPIDLabelProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerPIDLabel(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
void init_bf_pid(pybind11::module m);
#endif

#endif
/** @} */ // end of doxygen group 

