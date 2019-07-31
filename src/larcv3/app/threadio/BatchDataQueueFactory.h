/**
 * \file BatchDataQueueFactory.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchDataQueueFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHDATAQUEUEFACTORY_H
#define __LARCV3THREADIO_BATCHDATAQUEUEFACTORY_H

#include <iostream>
#include <map>
#include "BatchDataQueue.h"
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/base/larbys.h"

namespace larcv3 {
  class QueueProcessor;
  /**
     \class BatchDataQueueFactory
     User defined class BatchDataQueueFactory ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class BatchDataQueueFactory {
    friend QueueProcessor;
  public:

    BatchDataQueueFactory()
    {}

    ~BatchDataQueueFactory()
    {}

    static const BatchDataQueueFactory<T>& get();

    inline bool exist_queue(std::string name) const
    {
      auto iter = _queue_m.find(name);
      return iter != _queue_m.end();
    }
    
    bool is_next_ready() const;

    // Pop all queues to promote next to current
    void pop_all();

    const BatchDataQueue<T>& get_queue(std::string name) const;

    inline bool make_queue(std::string name)
    {
      if (exist_queue(name)) {
        LARCV_SERROR() << "Queue name " << name << " already present..." << std::endl;
        return false;
      }
      _queue_m.emplace(std::make_pair(name, BatchDataQueue<T>()));
      return true;
    }

    BatchDataQueue<T>& get_queue_writeable(std::string name);

  private:
    static BatchDataQueueFactory<T>& get_writeable();

  private:
    static BatchDataQueueFactory<T>* _me;
    std::map<std::string, BatchDataQueue<T> > _queue_m;
  };

}

#endif
/** @} */ // end of doxygen group

