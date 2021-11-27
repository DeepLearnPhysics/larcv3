/**
 * @file EventVoxel2D.h
 * @ingroup core_DataFormat
 * @brief Class def header for a class larcv3::EventSparseTensor and larcv3::EventClusterPixel2D
 * @author kazuhiro
 * @author cadams
 */
#pragma once

#ifdef LARCV_INTERNAL
	#include <pybind11/pybind11.h>
	template<size_t dimension>
	void init_eventsparse_tensor_base(pybind11::module m);
	void init_eventsparsetensor(pybind11::module m);
#endif

#include <iostream>

#include "EventBase.hh"
#include "DataProductFactory.hh"
#include "Voxel.hh"
#include "ImageMeta.hh"
#include "IOManager.hh"

/** @addtogroup DataFormat
 * @{
 */
namespace larcv3 
{
	/**
	 * @brief Event-wise class to store a collection of VoxelSet 
	 * (cluster) per projection id
	 * 
	 */
	template<size_t dimension>
	class EventSparseTensor : public EventBase 
	{
	public:
		/// Default constructor
		EventSparseTensor();

		/// Access to all stores larcv3::SparseTensor
		inline const std::vector<larcv3::SparseTensor<dimension>>& as_vector() const 
		{ return _tensor_v; }

		/// Access SparseTensor<dimension>  of a specific projection ID
		const larcv3::SparseTensor<dimension>& 
			sparse_tensor(const ProjectionID_t id) const;

		/// Number of valid projection id
		inline size_t size() const 
		{ return _tensor_v.size(); }

		inline larcv3::SparseTensor<dimension> at(size_t index) 
		{ return _tensor_v.at(index); }

		/// EventBase::clear() override
		void clear() 
		{}

		/// Emplace data
		void emplace(larcv3::SparseTensor<dimension> && voxels);
		/// Set data`
		void set(const larcv3::SparseTensor<dimension> & voxels);
		/// Emplace a new element
		void emplace(larcv3::VoxelSet&& voxels, larcv3::ImageMeta<dimension>&& meta);
		/// Set a new element
		void set(const larcv3::VoxelSet& voxels, const larcv3::ImageMeta<dimension>& meta);

		// IO functions:
		void initialize(hid_t group, uint compression);
		void serialize(hid_t group);
		void deserialize(hid_t group, size_t entry, bool reopen_groups=false);
		void finalize();

	private:
		void open_in_datasets(hid_t group);
		void open_out_datasets(hid_t group);
		std::vector<larcv3::SparseTensor<dimension>>  _tensor_v;
	};


	using EventSparseTensor2D = EventSparseTensor<2>;
	using EventSparseTensor3D = EventSparseTensor<3>;

	// Template instantiation for IO
	template<> inline std::string product_unique_name<larcv3::EventSparseTensor2D>() 
	{ return "sparse2d"; }

	template<> inline std::string product_unique_name<larcv3::EventSparseTensor3D>() 
	{ return "sparse3d"; }

	/**
	 * @brief A concrete factory class for larcv3::EventSparseTensor
	 * 
	 */
	template<size_t dimension>
	class EventSparseTensorFactory : public DataProductFactoryBase 
	{
	public:
		/// constructor
		EventSparseTensorFactory()
		{ DataProductFactory::get().add_factory(product_unique_name<larcv3::EventSparseTensor<dimension>>(), this); }
		
		/// destructor
		~EventSparseTensorFactory() 
		{}

		/// create method
		EventBase* create() 
		{ return new EventSparseTensor<dimension>; }
	};
}
/** @} */ // end of doxygen group