/**
 * @file BBox.hh
 * @ingroup core_DataFormat
 * @brief Class def header for a class larcv3::BBox2D and larcv3::BBox3D
 * @author kazuhiro, cadams
 */
#pragma once

#ifdef LARCV_INTERNAL
	#include <pybind11/pybind11.h>
	void init_bbox(pybind11::module m);
#endif

#include <iostream>

#include "DataFormatTypes.hh"

/** @addtogroup core_DataFormat
 * @{
 */
namespace larcv3 
{
	/**
	 * @brief Bounding box in ND
	 * 
	 */
	template<size_t dimension>
	class BBox
	{
	public:

		BBox();

		BBox(const std::array<double, dimension>& centroid, 
			 const std::array<double, dimension>& half_length,
			 const std::array<double, dimension*dimension> & rotation = {});

		std::string dump() const;
		std::array<double, dimension*dimension> identity_rotation();

		const std::array<double, dimension>& centroid() const 
		{return _centroid;} 

		const std::array<double, dimension>& half_length() const 
		{return _half_length;}

		const std::array<double, dimension*dimension> & rotation_matrix() const 
		{return _rotation;}

		inline bool operator== (const BBox<dimension>& rhs) const 
		{
			return (_centroid    == rhs._centroid 	  && 
					_half_length == rhs._half_length && 
					_rotation    == rhs._rotation);
		}

	private:

		///	central location of the bbox.  
		///	Rotations, if used, are around this point.
		std::array<double, dimension> _centroid;             

		/// half length of BBox in each dimension.
		std::array<double, dimension> _half_length;          

		/// unitary rotation matrix, defaults to identity.
		std::array<double, dimension*dimension> _rotation;  

	public:
#ifdef USE_HDF5
		static hid_t get_datatype() 
		{
			hid_t datatype;
			datatype = H5Tcreate (H5T_COMPOUND, sizeof (BBox));

			hsize_t array_dimensions[1];
			array_dimensions[0] = dimension;

			hid_t double_type   = H5Tarray_create(larcv3::get_datatype<double>(), 1, array_dimensions);

			hsize_t rotation_dimensions[1];
			rotation_dimensions[0] = dimension*dimension;
			hid_t rotation_type = H5Tarray_create(larcv3::get_datatype<double>(), 1, rotation_dimensions);

			H5Tinsert (datatype, "centroid",
						HOFFSET (BBox, _centroid),
						double_type);
			H5Tinsert (datatype, "half_length",
						HOFFSET (BBox, _half_length),
						double_type);
			H5Tinsert (datatype, "rotation",
						HOFFSET (BBox, _rotation),
						rotation_type);

			return datatype;
		}
#endif
	};

	using BBox2D = BBox<2>;
	using BBox3D = BBox<3>;

	/**
	 * @brief Bounding box collection for N dimensions
	 * 
	 */
	template<size_t dimension>
	class BBoxCollection
	{
		// template <size_t dimension> friend class EventBBox<dimension>;
		template<size_t> friend class EventBBox;
	public:
		BBoxCollection(){};

		/// Get # of BBox
		inline size_t size() const 
		{ return _bbox_v.size(); }

		/// Access specific BBox
		const larcv3::BBox<dimension> & bbox(InstanceID_t id) const 
		{return _bbox_v.at(id);}

		/// Access all BBox as a vector
		inline const std::vector<larcv3::BBox<dimension> >& as_vector() const
		{ return _bbox_v; }

		/// Clear everything
		inline void clear_data() { _bbox_v.clear(); }

		/// Resize voxel array
		inline void resize(const size_t num)
		{ this->clear_data(); _bbox_v.resize(num); }

		/// Access non-const reference of a specific BBox
		larcv3::BBox<dimension>& writeable_bbox(const InstanceID_t id) 
		{return _bbox_v.at(id);}

		/// Move a BBox into a collection.
		void emplace(larcv3::BBox<dimension> && bbox)
		{_bbox_v.push_back(bbox);}

		/// Set a BBox into a collection.
		void append(const larcv3::BBox<dimension> & bbox) 
		{_bbox_v.push_back(bbox);}

		/// Mover
		void move(larcv3::BBoxCollection<dimension> && orig)
		{ _bbox_v = std::move(orig._bbox_v); }

	private:
		std::vector<BBox<dimension> > _bbox_v;
	};

	typedef BBoxCollection<2> BBoxCollection2D;
	typedef BBoxCollection<3> BBoxCollection3D;
}
/** @} */ // end of doxygen group