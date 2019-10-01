/**
 * \file CompressAndEmbed.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class CompressAndEmbed
 *
 * @author cadams
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARCV_COMPRESSANDEMBED_H__
#define __LARCV_COMPRESSANDEMBED_H__

#include "larcv3/core/processor/ProcessBase.h"
#include "larcv3/core/processor/ProcessFactory.h"
// #include "larcv3/core/dataformat/Tensor.h"
namespace larcv3 {

  /**
     \class ProcessBase
     User defined class CompressAndEmbed ... these comments are used to generate
     doxygen documentation!
  */
  class CompressAndEmbed : public ProcessBase {

  public:

    /// Default constructor
    CompressAndEmbed(const std::string name = "CompressAndEmbed");

    /// Default destructor
    ~CompressAndEmbed() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<std::string> _producer_v;
    std::vector<std::string> _data_type_v;
    std::vector<size_t     > _row_compression_v;
    std::vector<size_t     > _col_compression_v;
    std::vector<size_t     > _output_rows_v;
    std::vector<size_t     > _output_cols_v;
    std::vector<size_t     > _mode_v;

  };

  /**
     \class larcv3::CompressAndEmbedFactory
     \brief A concrete factory class for larcv3::CompressAndEmbed
  */
  class CompressAndEmbedProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CompressAndEmbedProcessFactory() { ProcessFactory::get().add_factory("CompressAndEmbed", this); }
    /// dtor
    ~CompressAndEmbedProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CompressAndEmbed(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

