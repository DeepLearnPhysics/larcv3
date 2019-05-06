/**
 * \file BatchFillerMultiLabel.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerMultiLabel
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERMULTILABEL_H__
#define __LARCV3THREADIO_BATCHFILLERMULTILABEL_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerMultiLabel ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerMultiLabel : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerMultiLabel(const std::string name="BatchFillerMultiLabel");
    
    /// Default destructor
    ~BatchFillerMultiLabel(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void _batch_begin_();

    void _batch_end_();

    void finalize();

  private:
    std::string _part_producer;
    std::vector<float> _entry_data;
    size_t _num_class;
    std::vector<int> _pdg_list;
  };

  /**
     \class larcv3::BatchFillerMultiLabelFactory
     \brief A concrete factory class for larcv3::BatchFillerMultiLabel
  */
  class BatchFillerMultiLabelProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerMultiLabelProcessFactory() { ProcessFactory::get().add_factory("BatchFillerMultiLabel",this); }
    /// dtor
    ~BatchFillerMultiLabelProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerMultiLabel(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

