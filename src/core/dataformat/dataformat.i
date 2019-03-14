%module dataformat
%{
/* Put header files here or function declarations like below */
#include "DataFormatTypes.h"
#include "Point.h"
#include "Vertex.h"
#include "Particle.h"
#include "Voxel.h"
#include "EventBase.h"
#include "DataProductFactory.h"
#include "IOManager.h"
#include "EventParticle.h"

using namespace larcv;
%}
%include "larcv/core/base/base.i"
%include "std_string.i"
%include "DataFormatTypes.h"
%include "Point.h"
%include "Vertex.h"
%include "Particle.h"
%include "Voxel.h"
%include "EventBase.h"
%include "DataProductFactory.h"
%include "IOManager.h"
%include "EventParticle.h"

