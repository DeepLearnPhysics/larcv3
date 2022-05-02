#include "imagemod.h"

#include "Downsample.h"
#include "Threshold.h"
#include "TensorFromCluster.h"
#include "DenseToSparse.h"
#include "SparseToDense.h"
#include "BBoxFromParticle.h"
#include "BBoxFromCluster.h"
#include "Embed.h"


void init_imagemod(pybind11::module m){

  init_downsample(m);
  init_threshold(m);
  init_tensor_from_cluster(m);
  init_sparse_to_dense(m);
  init_dense_to_sparse(m);
  init_bbox_from_particle(m);
  init_bbox_from_cluster(m);
  init_embed(m);
}
