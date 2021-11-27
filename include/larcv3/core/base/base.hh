/**
 * @file base.hh
 * @ingroup core_Base
 * @brief Class def header for exception classes for larcv3 framework
 * @author cadams
 */
#pragma once

#include "larbys.hh"
#include "larcv_base.hh"
#include "PSet.hh"
#include "Watch.hh"

/** \addtogroup core_Base
 * @{
 */
#ifndef LARCV_NO_PYBIND
    #ifdef LARCV_INTERNAL
        #include <pybind11/pybind11.h>
        __attribute__ ((visibility ("default"))) void init_base(pybind11::module m);
    #endif
#endif
/** @} */ // end of group