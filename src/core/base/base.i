%module base
%{
/* Put header files here or function declarations like below */
#include "larcv/core/base/LArCVTypes.h"
#include "larcv/core/base/larcv_logger.h"
#include "larcv/core/base/larcv_base.h"
#include "larcv/core/base/LArCVBaseUtilFunc.h"
#include "larcv/core/base/Parser.h"
#include "larcv/core/base/PSet.h"
#include "larcv/core/base/Watch.h"
#include "larcv/core/base/ConfigManager.h"
using namespace larcv;
%}
%include "larcv/core/base/std_string.i"
%include "larcv/core/base/LArCVTypes.h"
%include "larcv/core/base/larcv_logger.h"
%include "larcv/core/base/larcv_base.h"
%include "larcv/core/base/LArCVBaseUtilFunc.h"
%include "larcv/core/base/Parser.h"
%include "larcv/core/base/PSet.h"
%include "larcv/core/base/Watch.h"
%include "larcv/core/base/ConfigManager.h"
