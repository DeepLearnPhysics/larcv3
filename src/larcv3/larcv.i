%module larcv
%{

#include "larcv3/core/base/base.i"
#include "larcv3/core/dataformat/dataformat.i"
#include "larcv3/core/processor/processor.i"
#include "larcv3/core/pyutil/pyutil.i"
#include "larcv3/core/cpputil/cpputil.i"
#include "larcv3/app/filter/filter.i"
#include "larcv3/app/threadio/threadio.i"

using namespace larcv3;
using namespace std;
%}

%include "core/base/base.wrapper"
%include "core/dataformat/dataformat.wrapper"
%include "core/processor/processor.wrapper"
%include "core/cpputil/cpputil.wrapper"
%include "core/pyutil/pyutil.wrapper"
%include "larcv3/app/filter/filter.wrapper"
%include "larcv3/app/threadio/threadio.wrapper"
