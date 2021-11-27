/**
 * @file Point.h
 * @ingroup DataFormat
 * @brief Class def header for a class Point
 * @author kazuhiro
 */
#pragma once

#ifdef LARCV_INTERNAL
	#include <pybind11/pybind11.h>
	template <size_t dimension>
	void init_point_base(pybind11::module  m);
	void init_point(pybind11::module  m);
#endif

#include <iostream>
#include <cmath>
#include <cassert>
#include <array>

/** @addtogroup DataFormat
 * @{
 */
namespace larcv3 
{
	/**
	 * @brief 
	 * 
	 */
    template<size_t dimension>
    class Point 
	{
    public:
        Point() 
		: x() 
		{}

        Point(std::array<double, dimension> xv)
		{
            for(size_t i = 0; i < dimension; i ++) {
				x[i] = xv[i];
			}
        }

        ~Point() 
		{}

        Point(const Point<dimension>& pt)
		{
            for(size_t i = 0; i < dimension; i ++) {
				x[i] = pt.x[i];
			}
        }

        std::array<double, dimension> x;

        inline bool operator==(const Point<dimension>& rhs) const 
		{
            bool eq = true;
            for(size_t i = 0; i < dimension; i ++) {
				eq = (eq && x[i] == rhs.x[i]);
			}
            return eq;
        }

        inline bool operator!=(const Point<dimension>& rhs) const 
		{ return !(rhs == (*this)); }

        inline Point& operator*= (const double rhs) 
		{
            for(size_t i = 0; i < dimension; i ++) {
				x[i] *= rhs;
			}
            return (*this);
        }

        inline Point& operator/= (const double rhs) 
		{
            for(size_t i = 0; i < dimension; i ++) {
				x[i] /= rhs;
			}
            return (*this);
        }

        inline Point& operator+= (const Point<dimension>& rhs) 
		{
            for(size_t i = 0; i < dimension; i ++) {
				x[i] += rhs.x[i];
			}
            return (*this);
        }

        inline Point& operator-= (const Point<dimension>& rhs) 
		{
            for(size_t i = 0; i < dimension; i ++) {
				x[i] -= rhs.x[i];
			}
            return (*this);
        }

        inline Point operator* (const double rhs) const 
		{
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) {
				new_x[i] = rhs * x[i];
			}
            return Point(new_x);
        }
        inline Point operator/ (const double rhs) const 
		{
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) {
				new_x[i] = x[i] / rhs;
			}
            return Point(new_x);
        }

        inline Point operator+ (const Point<dimension>& rhs) const 
		{
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) {
				new_x[i] = x[i] + rhs.x[i];
			}
            return Point(new_x);
        }

        inline Point operator- (const Point<dimension>& rhs) const 
		{
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) {
				new_x[i] = x[i] - rhs.x[i];
			}
            return Point(new_x);
        }

        inline double squared_distance(const Point& pt) const 
		{
            double res(0.0);
            for(size_t i = 0; i < dimension; i ++) {
				res += pow(x[i]-pt.x[i],2);
			}
            return res;
        }

        inline double distance(const Point& pt) const 
		{ return sqrt(squared_distance(pt)); }

        inline Point direction(const Point& pt) const 
		{
            Point res(pt - *this);
            res /= distance(pt); 
			return res;
        }
    };

	using Point2D = Point<2>;
	using Point3D = Point<3>;
}
/** @} */ // end of doxygen group