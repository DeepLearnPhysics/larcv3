%module larcv
%{

#include "larcv3/core/base/base.i"
#include "larcv3/core/dataformat/dataformat.i"
#include "larcv3/core/processor/processor.i"
#include "larcv3/core/pyutil/pyutil.i"
#include "larcv3/core/cpputil/cpputil.i"
#include "larcv3/app/filter/filter.i"
#include "larcv3/app/threadio/threadio.i"
#include "larcv3/app/imagemod/imagemod.i"


using namespace larcv3;
using namespace std;
%}

%include exception.i       

%exception {
    try {
        $action
    } catch(const larcv3::larbys & e) {
        SWIG_exception(SWIG_ValueError, "Larbys exception");
    } catch(const std::exception& e) {
        SWIG_exception(SWIG_UnknownError, "Standard exception");
    } catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

%include "core/base/base.wrapper"
%include "core/dataformat/dataformat.wrapper"
%include "core/processor/processor.wrapper"
%include "core/cpputil/cpputil.wrapper"
%include "core/pyutil/pyutil.wrapper"
%include "larcv3/app/filter/filter.wrapper"
%include "larcv3/app/threadio/threadio.wrapper"
%include "larcv3/app/imagemod/imagemod.wrapper"
