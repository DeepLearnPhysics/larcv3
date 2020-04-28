/**
 * \file BatchFillerVertex.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerVertex
 *
 * @author Marco Del Tutto
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERVERTEX_H__
#define __LARCV3THREADIO_BATCHFILLERVERTEX_H__

#include "larcv3/core/processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

namespace larcv3 {

  /**
     \class ProcessBase
     User defined class BatchFillerVertex ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerVertex : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerVertex(const std::string name="BatchFillerVertex");
    
    /// Default destructor
    ~BatchFillerVertex(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void _batch_begin_();

    void _batch_end_();

    void finalize();

  private:
    std::string _part_producer;
    std::vector<float> _entry_data;
    size_t _vtx_dim = 3; ///< The vertex dimension (ex.: 3 if 3D vertex)
    // std::vector<int> _pdg_list;
  };

  /**
     \class larcv3::BatchFillerVertexFactory
     \brief A concrete factory class for larcv3::BatchFillerVertex
  */
  class BatchFillerVertexProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerVertexProcessFactory() { ProcessFactory::get().add_factory("BatchFillerVertex",this); }
    /// dtor
    ~BatchFillerVertexProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerVertex(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

