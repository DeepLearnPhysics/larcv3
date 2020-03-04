/**
 * \file BatchData.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchData
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __LARCV3THREADIO_BATCHDATA_H
#define __LARCV3THREADIO_BATCHDATA_H

#include <iostream>
#include <vector>
#include "QueueIOTypes.h"
#include "larcv3/core/pyutil/PyUtils.h"

#include <pybind11/numpy.h>

namespace larcv3 {
  /**
     \class BatchData
     User defined class BatchData ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class BatchData {

  public:

    /// Default constructor
    BatchData()
      : _current_size(0)
      , _state(BatchDataState_t::kBatchStateUnknown)
    {}

    /// Default destructor
    ~BatchData() {}

    const std::vector<T>& data() const;

    // Writeable access to data:
    std::vector<T> & writeable_data() {return _data;}

    pybind11::array_t<T> pydata();

    inline const std::vector<int>& dim() const { return _dim; }
    inline const std::vector<int>& dense_dim() const { return _dense_dim; }

    // Data size is number of elements regardless of the size of each element
    size_t data_size(bool calculate=false) const;

    inline size_t current_data_size() const { return _current_size; }

    size_t entry_data_size() const;

    void set_dim(const std::vector<int>& dim);
    void set_dense_dim(const std::vector<int>& dense_dim);
    void set_entry_data(const std::vector<T>& entry_data);

    void reset();
    void reset_data();

    inline bool is_filled() const
    { return ( _state == BatchDataState_t::kBatchStateFilled ); }

    inline BatchDataState_t state() const
    { return _state; }

  private:
    // This holds the data for this instance, and is changed often
    std::vector<T>   _data;
    // This holds the dimensions of the container for readout data (including sparse),
    // and is static. _data is flattened and this provides reshaping information
    std::vector<int> _dim;
    // This holds the dense shape of this data, in the case that the data is sparse
    // In the case that the data is dense, this matches _dim.
    std::vector<int> _dense_dim;
    size_t _current_size;
    BatchDataState_t _state;
  };
}

template <class T>
void init_batchdata_(pybind11::module m);

void init_batchdata(pybind11::module m);

#endif
/** @} */ // end of doxygen group

