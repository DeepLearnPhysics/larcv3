%module dataformat
%{
/* Put header files here or function declarations like below */
#include "core/dataformat/DataFormatTypes.h"
#include "core/dataformat/Point.h"
#include "core/dataformat/Vertex.h"
#include "core/dataformat/Particle.h"
#include "core/dataformat/Voxel.h"
#include "core/dataformat/EventID.h"
#include "core/dataformat/EventBase.h"
#include "core/dataformat/DataProductFactory.h"
#include "core/dataformat/IOManager.h"
#include "core/dataformat/EventParticle.h"

using namespace larcv;
%}
%include "std_string.i"
%include "core/dataformat/DataFormatTypes.h"
%include "core/dataformat/Point.h"
%include "core/dataformat/Vertex.h"
%include "core/dataformat/Particle.h"
%include "core/dataformat/Voxel.h"
%include "core/dataformat/EventID.h"
%include "core/dataformat/EventBase.h"
%include "core/dataformat/DataProductFactory.h"
%include "core/dataformat/IOManager.h"
%include "core/dataformat/EventParticle.h"

