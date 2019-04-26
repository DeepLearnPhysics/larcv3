%module processor
%{
/* Put header files here or function declarations like below */
#include "core/processor/ProcessorTypes.h"
#include "core/processor/ProcessBase.h"
#include "core/processor/ProcessDriver.h"
#include "core/processor/ProcessFactory.h"

using namespace larcv3;
%}

%include "core/processor/ProcessorTypes.h"
%include "core/processor/ProcessBase.h"
%include "core/processor/ProcessDriver.h"
%include "core/processor/ProcessFactory.h"

