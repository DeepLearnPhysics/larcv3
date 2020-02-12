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
#ifndef __LARCV3PROCESSOR_PROCESSOR_H__
#define __LARCV3PROCESSOR_PROCESSOR_H__


#include "ProcessBase.h"
#include "ProcessDriver.h"


#ifndef LARCV_NO_PYBIND

__attribute__ ((visibility ("default"))) void init_processor(pybind11::module m);


// bindings
#endif


// include guards
#endif