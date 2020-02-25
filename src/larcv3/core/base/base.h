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
#ifndef __LARCV3BASE_BASE_H__
#define __LARCV3BASE_BASE_H__


#include "larbys.h"
#include "larcv_base.h"
#include "PSet.h"
#include "Watch.h"

#ifndef LARCV_NO_PYBIND

__attribute__ ((visibility ("default"))) void init_base(pybind11::module m);

// bindings
#endif


// include guards
#endif