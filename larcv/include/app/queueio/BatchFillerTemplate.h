/**
 * \file BatchFillerTemplate.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerTemplate
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHFILLERTEMPLATE_H__
#define __LARCV3THREADIO_BATCHFILLERTEMPLATE_H__

#include "BatchHolder.h"
#include "BatchData.h"

namespace larcv3 {
  class ThreadProcessor; 
  class QueueProcessor;
  /**
     \class ProcessBase
     User defined class BatchFillerTemplate ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class BatchFillerTemplate : public BatchHolder {
    friend ThreadProcessor;
    friend QueueProcessor;
  public:

    /// Default constructor
    BatchFillerTemplate(const std::string name = "BatchFillerTemplate")
      : BatchHolder(name)
    {}

    /// Default destructor
    virtual ~BatchFillerTemplate() {}

    inline void batch_begin() {
      _batch_data_ptr->reset_data();
      _batch_begin_();
    }

    inline void batch_end()
    {
      if (!_batch_data_ptr->is_filled()) {
        LARCV_CRITICAL() << "Batch data is not filled @ end-of-batch (" << _batch_data_ptr->current_data_size()
                         << "/" << _batch_data_ptr->data_size() << ")!" << std::endl;
        throw larbys();
      }
      _batch_end_();
    }

    inline const BatchData<T>& batch_data() const { return *(_batch_data_ptr); }

    BatchDataType_t data_type() const;

    inline size_t type_size() const { return sizeof(T); }

  protected:

    inline void set_dim(std::vector<int> dim) {_batch_data_ptr->set_dim(dim);}
    inline void set_dense_dim(std::vector<int> dense_dim) {_batch_data_ptr->set_dense_dim(dense_dim);}
    inline void set_entry_data(const std::vector<T>& data)
    { _batch_data_ptr->set_entry_data(data); }

    virtual void _batch_begin_() =0;
    virtual void _batch_end_()   =0;

  private:

    BatchData<T>* _batch_data_ptr;

  };

}

#endif
/** @} */ // end of doxygen group

