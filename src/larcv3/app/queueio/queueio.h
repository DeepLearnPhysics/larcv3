
/**
 * \file larbys.h
 *
 * \ingroup core_Base
 * 
 * \brief Class def header for exception classes for larcv3 framework
 *
 * @author cadams
 */

/** \addtogroup core_Base

    @{*/
#ifndef __LARCV3QUEUEIO_QUEUEIO_H__
#define __LARCV3QUEUEIO_QUEUEIO_H__


#include "BatchData.h"
#include "QueueProcessor.h"

#ifndef LARCV_NO_PYBIND

void init_queueio(pybind11::module m);

// bindings
#endif


// include guards
#endif