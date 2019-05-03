%module base
%{
/* Put header files here or function declarations like below */
#include "larcv3/core/base/LArCVTypes.h"
#include "larcv3/core/base/larcv_logger.h"
#include "larcv3/core/base/larcv_base.h"
#include "larcv3/core/base/LArCVBaseUtilFunc.h"
#include "larcv3/core/base/Parser.h"
#include "larcv3/core/base/PSet.h"
#include "larcv3/core/base/Watch.h"
#include "larcv3/core/base/ConfigManager.h"
#include "larcv3/core/base/larbys.h"

using namespace larcv3;
using namespace std;
%}
%include <std_string.i>
%include <std_vector.i>

%include "larcv3/core/base/LArCVTypes.h"
%include "larcv3/core/base/larcv_logger.h"
%include "larcv3/core/base/larcv_base.h"
%include "larcv3/core/base/LArCVBaseUtilFunc.h"
%include "larcv3/core/base/Parser.h"
%include "larcv3/core/base/PSet.h"
%include "larcv3/core/base/Watch.h"
%include "larcv3/core/base/ConfigManager.h"
%include "larcv3/core/base/larbys.h"

