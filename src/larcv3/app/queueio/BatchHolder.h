/**
 * \file BatchHolder.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchHolder
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHHOLDER_H__
#define __LARCV3THREADIO_BATCHHOLDER_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "ThreadIOTypes.h"
namespace larcv3 {
  class ThreadProcessor;
  class QueueProcessor;

  /**
     \class ProcessBase
     User defined class BatchHolder ... these comments are used to generate
     doxygen documentation!
  */
  class BatchHolder : public ProcessBase{
    friend class ThreadProcessor;
    friend class QueueProcessor;
  public:
    
    /// Default constructor
    BatchHolder(const std::string name="BatchFiller")
      : ProcessBase(name)
      , _batch_size(0)
    {}
    
    /// Default destructor
    virtual ~BatchHolder(){}

    inline size_t batch_size() const { return _batch_size; }

    virtual BatchDataType_t data_type() const = 0;

    inline bool is(const std::string question) const
    { return (question == "BatchFiller"); }

  private:
    size_t _batch_size;
  };

}

#endif
/** @} */ // end of doxygen group 

