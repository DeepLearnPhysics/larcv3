%module base
%{
/* Put header files here or function declarations like below */
#include "core/base/LArCVTypes.h"
#include "core/base/larcv_logger.h"
#include "core/base/larcv_base.h"
#include "core/base/LArCVBaseUtilFunc.h"
#include "core/base/Parser.h"
#include "core/base/PSet.h"
#include "core/base/Watch.h"
#include "core/base/ConfigManager.h"
#include "core/base/larbys.h"

using namespace larcv3;
using namespace std;
%}
%include <std_string.i>
%include <std_vector.i>

%include "core/base/LArCVTypes.h"
%include "core/base/larcv_logger.h"
%include "core/base/larcv_base.h"
%include "core/base/LArCVBaseUtilFunc.h"
%include "core/base/Parser.h"
%include "core/base/PSet.h"
%include "core/base/Watch.h"
%include "core/base/ConfigManager.h"
%include "core/base/larbys.h"

