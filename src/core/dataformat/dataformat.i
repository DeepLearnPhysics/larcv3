%module dataformat
%{
/* Put header files here or function declarations like below */
#include "DataFormatTypes.h"
#include "Point.h"
#include "Vertex.h"
#include "Particle.h"
#include "Voxel.h"
using namespace larcv;
%}
%include "std_string.i"
%include "DataFormatTypes.h"
%include "Point.h"
%include "Vertex.h"
%include "Particle.h"
%include "Voxel.h"
