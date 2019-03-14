%module processor
%{
/* Put header files here or function declarations like below */
#include "ProcessorTypes.h"
#include "ProcessBase.h"
#include "ProcessDriver.h"
#include "ProcessFactory.h"

using namespace larcv;
%}

%include "ProcessorTypes.h"
%include "ProcessBase.h"
%include "ProcessDriver.h"
%include "ProcessFactory.h"

