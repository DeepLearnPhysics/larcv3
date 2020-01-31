// /**
//  * \file BatchFillerSparseCluster2D.h
//  *
//  * \ingroup ThreadIO
//  *
//  * \brief Class def header for a class BatchFillerSparseCluster2D
//  *
//  * @author cadams
//  */

// /** \addtogroup ThreadIO

//     @{*/
// #ifndef __BATCHFILLERSPARSECLUSTER2D_H__
// #define __BATCHFILLERSPARSECLUSTER2D_H__

// #include "core/Processor/ProcessFactory.h"
// #include "BatchFillerTemplate.h"
// #include "RandomCropper.h"
// #include "core/DataFormat/EventVoxel2D.h"

// namespace larcv3 {

//   /**
//      \class ProcessBase
//      User defined class BatchFillerCluster2D ... these comments are used to generate
//      doxygen documentation!
//   */
//   class BatchFillerSparseCluster2D : public BatchFillerTemplate<float> {

//   public:

//     /// Default constructor
//     BatchFillerSparseCluster2D(const std::string name="BatchFillerSparseCluster2D");

//     /// Default destructor
//     ~BatchFillerSparseCluster2D(){}

//     void configure(const PSet&);

//     void initialize();

//     bool process(IOManager& mgr);

//     void finalize();

//   protected:

//     void _batch_begin_();
//     void _batch_end_();

//   private:

//     size_t set_data_size(const larcv3::EventClusterPixel2D& image_data);
//     void assert_dimension(const larcv3::EventClusterPixel2D& image_data) const;

//     std::string _cluster2d_producer;
//     size_t _max_voxels;
//     float _unfilled_voxel_value;

//     std::vector<float>  _entry_data;
//     bool _allow_empty;
//     bool _include_values;
//     bool _augment;


//     std::vector<size_t> _slice_v;
//     size_t _num_channels;



//   };

//   /**
//      \class larcv3::BatchFillerSparseCluster2DFactory
//      \brief A concrete factory class for larcv3::BatchFillerSparseCluster2D
//   */
//   class BatchFillerSparseCluster2DProcessFactory : public ProcessFactoryBase {
//   public:
//     /// ctor
//     BatchFillerSparseCluster2DProcessFactory() { ProcessFactory::get().add_factory("BatchFillerSparseCluster2D",this); }
//     /// dtor
//     ~BatchFillerSparseCluster2DProcessFactory() {}
//     /// creation method
//     ProcessBase* create(const std::string instance_name) {
//       return new BatchFillerSparseCluster2D(instance_name);
//     }
//   };

// }

// #endif
// /** @} */ // end of doxygen group

