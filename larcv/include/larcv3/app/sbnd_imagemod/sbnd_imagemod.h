
/**
 * \file larbys.h
 *
 * \ingroup app_imagemod
 *
 * \brief Class def header for pybind11 for larcv3 framework
 *
 * @author cadams
 */

/** \addtogroup core_Base

    @{*/
#ifndef __LARCV3SBND_IMAGEMOD_IMAGEMOD_H__
#define __LARCV3SBND_IMAGEMOD_IMAGEMOD_H__



#ifndef LARCV_NO_PYBIND
#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
__attribute__ ((visibility ("default"))) void init_sbnd_imagemod(pybind11::module m);
#endif
// bindings
#endif


// include guards
#endif
