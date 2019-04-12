%module dataformat
%{
/* Put header files here or function declarations like below */

// Basic Types:
#include "core/dataformat/DataFormatTypes.h"
#include "core/dataformat/EventID.h"
#include "core/dataformat/Point.h"
#include "core/dataformat/BBox.h"
#include "core/dataformat/Vertex.h"
#include "core/dataformat/Particle.h"
#include "core/dataformat/ImageMeta.h"
#include "core/dataformat/Image2D.h"
#include "core/dataformat/Voxel.h"

// IO Interface classes:
#include "core/dataformat/DataProductFactory.h"
#include "core/dataformat/EventBase.h"
#include "core/dataformat/EventParticle.h"
#include "core/dataformat/EventImage2D.h"
#include "core/dataformat/IOManager.h"
#include "core/dataformat/EventVoxel.h"


using namespace larcv;
%}


%include "std_string.i"
%include "std_vector.i"

// Basic Types:
%include "core/dataformat/DataFormatTypes.h"
%include "core/dataformat/EventID.h"
%include "core/dataformat/Point.h"
%include "core/dataformat/BBox.h"
%include "core/dataformat/Vertex.h"
%include "core/dataformat/Particle.h"
%include "core/dataformat/ImageMeta.h"
%include "core/dataformat/Image2D.h"
%include "core/dataformat/Voxel.h"

// IO Interface classes:
%include "core/dataformat/DataProductFactory.h"
%include "core/dataformat/EventBase.h"
%include "core/dataformat/EventParticle.h"
%include "core/dataformat/EventImage2D.h"
%include "core/dataformat/IOManager.h"
%include "core/dataformat/EventVoxel.h"


// Define instantiations of LARCV templates:
%template(Point2D) larcv::Point<2>;
%template(Point3D) larcv::Point<3>;
%template(BBox2D) larcv::BBox<2>;
%template(BBox3D) larcv::BBox<3>;
%template(ImageMeta2D) larcv::ImageMeta<2>;
%template(ImageMeta3D) larcv::ImageMeta<3>;
%template(SparseTensor2D) larcv::SparseTensor<2>;
%template(SparseTensor3D) larcv::SparseTensor<3>;
%template(EventSparseTensor2D) larcv::EventSparseTensor<2>;
%template(EventSparseTensor3D) larcv::EventSparseTensor<3>;
%template(SparseCluster2D) larcv::SparseCluster<2>;
%template(SparseCluster3D) larcv::SparseCluster<3>;
%template(EventSparseCluster2D) larcv::EventSparseCluster<2>;
%template(EventSparseCluster3D) larcv::EventSparseCluster<3>;


// Add templates for the dataproducts that get wrapped vectors:
%template(VectorOfDouble) std::vector<double>;
%template(VectorOfFloat) std::vector<float>;
%template(VectorOfSizet) std::vector<size_t>;
%template(VectorOfVoxel) std::vector<larcv::Voxel>;
%template(VectorOfVoxelSet) std::vector<larcv::VoxelSet>;
%template(VectorOfSparse2D) std::vector<larcv::SparseTensor2D>;
%template(VectorOfSparse3D) std::vector<larcv::SparseTensor3D>;
%template(VectorOfCluster2D) std::vector<larcv::SparseCluster2D>;
%template(VectorOfCluster3D) std::vector<larcv::SparseCluster3D>;
%template(VectorOfParticle) std::vector<larcv::Particle>;
%template(VectorOfImage2D) std::vector<larcv::Image2D>;



