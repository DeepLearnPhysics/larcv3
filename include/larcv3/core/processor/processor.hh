/**
 * @file processor.hh
 * @ingroup core_Processor
 * @brief 
 * @author 
 */
#pragma once

#include "ProcessBase.hh"
#include "ProcessDriver.hh"

/** @addtogroup core_Processor
 * @{
 */
#ifndef LARCV_NO_PYBIND
    #ifdef LARCV_INTERNAL
        #include <pybind11/pybind11.h>
        __attribute__ ((visibility ("default"))) void init_processor(pybind11::module m);
    #endif
#endif
/** @} */ // end of group