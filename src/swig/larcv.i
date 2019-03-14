%module larcv
%{
/* Put header files here or function declarations like below */
// Files included from base:
#include "larcv/core/base/LArCVTypes.h"
#include "larcv/core/base/larcv_logger.h"
#include "larcv/core/base/larcv_base.h"
#include "larcv/core/base/LArCVBaseUtilFunc.h"
#include "larcv/core/base/Parser.h"
#include "larcv/core/base/PSet.h"
#include "larcv/core/base/Watch.h"
#include "larcv/core/base/ConfigManager.h"

// Files included from dataformat:
#include "larcv/core/dataformat/DataFormatTypes.h"
#include "larcv/core/dataformat/Point.h"
#include "larcv/core/dataformat/Vertex.h"
#include "larcv/core/dataformat/Particle.h"
#include "larcv/core/dataformat/Voxel.h"
#include "larcv/core/dataformat/EventBase.h"
#include "larcv/core/dataformat/DataProductFactory.h"
#include "larcv/core/dataformat/IOManager.h"
#include "larcv/core/dataformat/EventParticle.h"

// Files included from processor:

using namespace larcv;
%}
%include "std_string.i"

%include "larcv/core/base/LArCVTypes.h"
%include "larcv/core/base/larcv_logger.h"
%include "larcv/core/base/larcv_base.h"
%include "larcv/core/base/LArCVBaseUtilFunc.h"
%include "larcv/core/base/Parser.h"
%include "larcv/core/base/PSet.h"
%include "larcv/core/base/Watch.h"
%include "larcv/core/base/ConfigManager.h"


%include "larcv/core/dataformat/DataFormatTypes.h"
%include "larcv/core/dataformat/Point.h"
%include "larcv/core/dataformat/Vertex.h"
%include "larcv/core/dataformat/Particle.h"
%include "larcv/core/dataformat/Voxel.h"
%include "larcv/core/dataformat/EventBase.h"
%include "larcv/core/dataformat/DataProductFactory.h"
%include "larcv/core/dataformat/IOManager.h"
%include "larcv/core/dataformat/EventParticle.h"