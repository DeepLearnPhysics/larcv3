#ifndef __EMBED_CXX__
#define __EMBED_CXX__

#include "Embed.h"
#include "larcv3/core/dataformat/EventTensor.h"
#include "larcv3/core/dataformat/EventSparseTensor.h"
#include "larcv3/core/dataformat/EventSparseCluster.h"

#include <numeric>

namespace larcv3 {

static EmbedProcessFactory __global_EmbedProcessFactory__;

 
Embed::Embed(const std::string name)
    : ProcessBase(name) {}


 
void Embed::configure(const json& cfg) {

  config = this -> default_config();
  config = augment_default_config(config, cfg);

  // We're really just verifying that everything is a filled
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  if (producer.empty()) {
    LARCV_CRITICAL() << "Must specify a producer" << std::endl;
    throw larbys();
  }
  if (product.empty()) {
    LARCV_CRITICAL() << "Must specify a Product" << std::endl;
    throw larbys();
  }
  if (output_producer.empty()) {
    LARCV_CRITICAL() << "Must specify an OutputProducer" << std::endl;
    throw larbys();
  }


}

 
void Embed::initialize() {}

 
bool Embed::process(IOManager& mgr) {

  //We apply thresholding for each projection id.

  // std::cout << "Enter Embed::process " << std::endl;
  auto const& producer        = config["Producer"].get<std::string>();
  auto const& product         = config["Product"].get<std::string>();
  auto const& output_producer = config["OutputProducer"].get<std::string>();

  auto const& target_size     = config["TargetSize"].get<std::vector<int>>();


  if (product == "sparse2d") 
    process_sparse_product<2>(mgr, producer, output_producer, target_size);
  else if (product == "sparse3d")
    process_sparse_product<3>(mgr, producer, output_producer, target_size);
  else if (product == "tensor1d")
    process_dense_product<1>(mgr, producer, output_producer, target_size);
  else if (product == "tensor2d")
    process_dense_product<2>(mgr, producer, output_producer, target_size);
  else if (product == "tensor3d")
    process_dense_product<3>(mgr, producer, output_producer, target_size);
  else if (product == "tensor4d")
    process_dense_product<4>(mgr, producer, output_producer, target_size);
  else{
    LARCV_CRITICAL() << "Can't apply embedding to product " << product << std::endl;
    throw larbys();
  }

  return true;
}

template <size_t dimension>
std::vector<int> Embed::create_new_image_meta_and_offsets(
      const ImageMeta<dimension> & original_meta, 
      const std::vector<int> & target_size,
      ImageMeta<dimension> & new_meta){
  // Returning the new meta by reference

  // How many pixels in the new meta?
  int prod_target_size = std::accumulate(
    target_size.begin(), target_size.end(), 1, std::multiplies<int>());

  // Make sure the target size is the right dimension:
  if (target_size.size() != original_meta.n_dims()){
    LARCV_CRITICAL() << "Can not embed image of dimension " << original_meta.n_dims() 
                     << " into new image of dimension " << target_size.size() << std::endl;
    throw larbys();
  }

  // Validate that the new target size is larger than the old one
  if (original_meta.total_voxels() > prod_target_size){
    LARCV_CRITICAL() << "Can not embed image of size " << original_meta.total_voxels() 
                     << " into new image of size " << prod_target_size << std::endl;
    throw larbys();
  }

  std::vector<int> offsets; offsets.resize(target_size.size());
  // Update each dimension:
  for (size_t i_dim = 0; i_dim < original_meta.n_dims(); i_dim ++){
    // Voxel size is unchanged.
    int total_offset = target_size.at(i_dim) - original_meta.number_of_voxels(i_dim);
    offsets.at(i_dim) = total_offset / 2; // Deliberately allowing truncating here

    // New size is the original voxel size * N new voxels
    double new_image_size = original_meta.voxel_dimensions(i_dim) * target_size.at(i_dim);

    // Origin is the old origin - offset:
    double new_origin = original_meta.origin(i_dim) - original_meta.voxel_dimensions(i_dim)*offsets.at(i_dim);

    // Assign everything
    new_meta.set_dimension(i_dim, new_image_size, target_size.at(i_dim), new_origin);
  }
  return offsets;
}

template< size_t dimension>
bool Embed::process_sparse_product(IOManager& mgr, std::string producer, 
                                      std::string output_producer,
                                      std::vector<int> target_size){

  // Get the input data and the output holder.
  auto const & ev_input  = mgr.template get_data<EventSparseTensor<dimension>>(producer);
  auto       & ev_output = mgr.template get_data<EventSparseTensor<dimension>>(output_producer);


  // Loop over the projection IDs
  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){

    // Get the old image:
    const auto& original_image = ev_input.as_vector().at(i) ;
    // Get the old meta:
    const auto& original_meta = original_image.meta();



    // The new meta will have the same voxel size and dimensions as the old meta.
    auto new_meta = original_meta;
    auto offsets = create_new_image_meta_and_offsets<dimension>(
      original_meta, target_size, new_meta);


    std::cout << original_meta.dump() << std::endl;
    std::cout << new_meta.dump() << std::endl;


    // Loop throught the objects and put all the pixels/voxels into the new positions:
    auto new_voxel_set = SparseTensor<dimension>();

    auto original_values  = original_image.values_vec();
    auto original_indexes = original_image.indexes_vec();

    for (size_t i_voxel = 0; i_voxel < original_indexes.size(); i_voxel ++){
      
      // For each pixel in the original image, we calculate it's original multi-index:

      auto coords = original_meta.coordinates(original_indexes.at(i_voxel));
      
      // Add the offset to the coordinate:
      for (int i_coord = 0; i_coord < coords.size(); i_coord ++) 
        coords[i_coord] += offsets[i_coord];
      
      // Get the new index:
      auto new_index = new_meta.index(coords);

      // Update the new image:
      new_voxel_set.add(std::move(Voxel(new_index, original_values.at(i_voxel))));
    }

    ev_output.emplace(std::move(new_voxel_set), std::move(new_meta));
  }

  return true;
}

template< size_t dimension>
bool Embed::process_dense_product(IOManager& mgr, std::string producer, 
                                   std::string output_producer,
                                   std::vector<int> target_size){

  // Get the input data and the output holder.
  auto const & ev_input  = mgr.template get_data<EventTensor<dimension>>(producer);
  auto       & ev_output = mgr.template get_data<EventTensor<dimension>>(output_producer);


  // Loop over the projection IDs
  for (size_t i = 0; i < ev_input.as_vector().size(); i ++ ){

    // Get the old image:
    const auto& original_image = ev_input.as_vector().at(i) ;
    // Get the old meta:
    const auto& original_meta = original_image.meta();



    // The new meta will have the same voxel size and dimensions as the old meta.
    auto new_meta = original_meta;
    auto offsets = create_new_image_meta_and_offsets<dimension>(
      original_meta, target_size, new_meta);


    // Loop throught the objects and put all the pixels/voxels into the new positions:
    auto new_image = Tensor<dimension>(new_meta);

    for (size_t i_pixel = 0; i_pixel < original_meta.total_voxels(); i_pixel ++){
      // Initializes to 0.0;  Skip if original is 0:
      if (original_image.pixel(i_pixel) == 0.0) continue;

      // For each pixel in the original image, we calculate it's original multi-index:
      auto coords = original_meta.coordinates(i_pixel);
      // Add the offset to the coordinate:
      for (int i_coord = 0; i_coord < coords.size(); i_coord ++) 
        coords[i_coord] += offsets[i_coord];


      // Update the new image:
      new_image.set_pixel(coords, original_image.pixel(i_pixel));
    }

    ev_output.emplace(std::move(new_image));
  }

  return true;
}
 
void Embed::finalize() {}

} //namespace larcv3


#include <pybind11/stl.h>



void init_embed(pybind11::module m){


  using Class = larcv3::Embed;
  pybind11::class_<Class> Embed(m, "Embed");
  // pybind11::class_<Class, std::shared_ptr<Class>> ev_sparse_tensor(m, classname.c_str());

  Embed.def(pybind11::init<std::string>(),
    pybind11::arg("name") = "Embed");

  Embed.def("default_config", &Class::default_config);
}

#endif






