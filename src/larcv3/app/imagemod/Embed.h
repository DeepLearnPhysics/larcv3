/**
 * \file Embed.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Embed
 *
 * Embed an object into a bigger image, without downsampling.
 * 
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LARCV3_EMBED_H__
#define __LARCV3_EMBED_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
#include "larcv3/core/dataformat/ImageMeta.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class embed ... these comments are used to generate
     doxygen documentation!
  */
  class Embed : public ProcessBase {

  public:

    /// Default constructor
    Embed(const std::string name="Embed");

    /// Default destructor
    ~Embed(){}

    void configure(const json&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();


    static json default_config(){
        json c = {
          {"Producer", std::string()},
          {"Product", std::string()},
          {"OutputProducer", std::string()},
          {"TargetSize", std::vector<int>()},
        };
        return c;
    }


  private:

    template< size_t dimension>
    bool process_dense_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        const std::vector<int>& target_size);

    template< size_t dimension>
    bool process_sparse_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        const std::vector<int>&  target_size);

    template< size_t dimension>
    bool process_bbox_product(
        IOManager& mgr,
        std::string producer, 
        std::string output_producer,
        const std::vector<int>& target_size);

    template< size_t dimension>
    bool process_cluster_product(
        IOManager& mgr, 
        std::string producer,
        std::string output_producer,
        const std::vector<int>& target_size);

    template< size_t dimension>
    std::vector<int> create_new_image_meta_and_offsets(
      const ImageMeta<dimension> & input, 
      const std::vector<int> & target_size,
      ImageMeta<dimension> & new_meta);

    json config;

  };


  /**
     \class larcv3::ThresholdFactory
     \brief A concrete factory class for larcv3::Embed
  */
  class EmbedProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EmbedProcessFactory() { ProcessFactory::get().add_factory("Embed",this); }
    /// dtor
    ~EmbedProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Embed(instance_name); }
  };

}

#ifdef LARCV_INTERNAL
#include <pybind11/pybind11.h>
void init_embed(pybind11::module m);
#endif


#endif
/** @} */ // end of doxygen group




  