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


#include "ProcessBase.hh"
#include "ProcessDriver.hh"


#ifndef LARCV_NO_PYBIND
#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
__attribute__ ((visibility ("default"))) void init_processor(pybind11::module m);
#endif

// bindings
#endif


// include guards
#endif
