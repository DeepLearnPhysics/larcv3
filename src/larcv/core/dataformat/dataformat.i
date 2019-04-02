%module dataformat
%{
/* Put header files here or function declarations like below */
#include "core/dataformat/DataFormatTypes.h"
#include "core/dataformat/Point.h"
#include "core/dataformat/Vertex.h"
#include "core/dataformat/Particle.h"
#include "core/dataformat/Voxel.h"
#include "core/dataformat/ImageMeta.h"
#include "core/dataformat/EventID.h"
#include "core/dataformat/EventBase.h"
#include "core/dataformat/DataProductFactory.h"
#include "core/dataformat/IOManager.h"
#include "core/dataformat/EventParticle.h"
#include "core/dataformat/EventVoxel.h"

using namespace larcv;
%}
%include "std_string.i"
%include "std_vector.i"
%include "core/dataformat/DataFormatTypes.h"
%include "core/dataformat/Point.h"
%include "core/dataformat/Vertex.h"
%include "core/dataformat/Particle.h"
%include "core/dataformat/Voxel.h"
%include "core/dataformat/ImageMeta.h"
%include "core/dataformat/EventID.h"
%include "core/dataformat/EventBase.h"
%include "core/dataformat/DataProductFactory.h"
%include "core/dataformat/IOManager.h"
%include "core/dataformat/EventParticle.h"
%include "core/dataformat/EventVoxel.h"

// Add templates for the dataproducts that get wrapped vectors:
%template(VectorOfDouble) std::vector<double>;
%template(VectorOfFloat) std::vector<float>;
%template(VectorOfSizet) std::vector<size_t>;
%template(VectorOfVoxel) std::vector<larcv::Voxel>;
%template(VectorOfVoxelSet) std::vector<larcv::VoxelSet>;
%template(VectorOfSparse) std::vector<larcv::SparseTensor>;
%template(VectorOfParticle) std::vector<larcv::Particle>;

