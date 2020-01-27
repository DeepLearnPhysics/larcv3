/**
 * \file BatchDataQueue.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchDataQueue
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHDATAQUEUE_H
#define __LARCV3THREADIO_BATCHDATAQUEUE_H

#include <iostream>
#include "BatchData.h"

namespace larcv3 {
  /**
     \class BatchDataQueue
     User defined class BatchDataQueue ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class BatchDataQueue {
    
  public:
    /// Default constructor
    BatchDataQueue();

    /// Default destructor
    ~BatchDataQueue(){}

    void reset();

    // Return detailed state of the next batch
    inline BatchDataState_t next_state() const
    { return _data_next.state(); }

    // Return whether the next batch of data is ready to go or not:
    bool is_next_ready () const;

    // Access to the current batch of data
    // Returns const reference and it is expected that _data_current will persist until
    // pop is called.
    const BatchData<T>& get_batch  () const;

    // Pop moves _data_next into _data_current and resets _data_next.
    void pop();


    // Writeable access to the next batch of data
    BatchData<T>& get_next_writeable();

    // Set the data for the next batch
    void set_next_data  (const std::vector<T>& source);


  private:
    larcv3::BatchData<T> _data_current;
    larcv3::BatchData<T> _data_next;

  };
  
}

#endif
/** @} */ // end of doxygen group 

