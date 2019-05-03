%module pyutil


%{

/* Put header files here or function declarations like below */
#include "larcv3/core/pyutil/PyUtils.h"
#include "larcv3/core/dataformat/ImageMeta.h"

%}

// Handle vector overload conversion:
%rename(as_ndarray_double) larcv3::as_ndarray(const std::vector<double> & );
%rename(as_ndarray_float)  larcv3::as_ndarray(const std::vector<float> & );
%rename(as_ndarray_int)    larcv3::as_ndarray(const std::vector<int> & );
%rename(as_ndarray_uint)   larcv3::as_ndarray(const std::vector<unsigned int> & );

%include "larcv3/core/pyutil/PyUtils.h"
%include "larcv3/core/dataformat/ImageMeta.h"



// short             
// unsigned short    
// int               
// unsigned int      
// long long         
// unsigned long long
// float             
// double            