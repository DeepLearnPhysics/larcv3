%module base
%{
/* Put header files here or function declarations like below */
#include "LArCVTypes.h"
#include "larcv_logger.h"
#include "larcv_base.h"
#include "LArCVBaseUtilFunc.h"
#include "Parser.h"
#include "PSet.h"
#include "Watch.h"
#include "ConfigManager.h"
using namespace larcv;
%}
%include "LArCVTypes.h"
%include "larcv_logger.h"
%include "larcv_base.h"
%include "LArCVBaseUtilFunc.h"
%include "Parser.h"
%include "PSet.h"
%include "Watch.h"
%include "ConfigManager.h"
