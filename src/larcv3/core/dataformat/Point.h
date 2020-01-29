/**
 * \file Point.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class Point
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV3DATAFORMAT_POINT_H
#define __LARCV3DATAFORMAT_POINT_H

#include <iostream>
#include <cmath>
#include <cassert>
#include <pybind11/pybind11.h>


namespace larcv3 {

    template<size_t dimension>
    class Point {
    public:
        Point() : x() { };
        Point(std::array<double, dimension> xv){
            for(size_t i = 0; i < dimension; i ++) x[i] = xv[i];
        }
        ~Point() {}

        template<size_t other_dim>
        Point(const Point<other_dim>& pt){
            assert(dimension == other_dim);
            for(size_t i = 0; i < dimension; i ++) x[i] = pt[i];
        };

        std::array<double, dimension> x;

        template<size_t other_dim>
        inline bool operator== (const Point<other_dim>& rhs) const {
            assert(dimension == other_dim);
            bool eq = true;
            for(size_t i = 0; i < dimension; i ++) eq = (eq && x[i] == rhs.x[i]);
            return eq;
        }

        template<size_t other_dim>
        inline bool operator!= (const Point<other_dim>& rhs) const {
            assert(dimension == other_dim); return !(rhs == (*this));
        }

        inline Point& operator*= (const double rhs) {
            for(size_t i = 0; i < dimension; i ++) x[i] *= rhs;
            return (*this);
        }

        inline Point& operator/= (const double rhs) {
            for(size_t i = 0; i < dimension; i ++) x[i] /= rhs;
            return (*this);
        }

        template<size_t other_dim>
        inline Point& operator+= (const Point<other_dim>& rhs) {
            assert(dimension == other_dim);
            for(size_t i = 0; i < dimension; i ++) x[i] += rhs.x[i];
            return (*this);
        }

        template<size_t other_dim>
        inline Point& operator-= (const Point<other_dim>& rhs) {
            assert(dimension == other_dim);
            for(size_t i = 0; i < dimension; i ++) x[i] -= rhs.x[i];
            return (*this);
        }

        inline Point operator* (const double rhs) const {
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) new_x[i] = rhs * x[i];
            return Point(new_x);
        }
        inline Point operator/ (const double rhs) const {
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) new_x[i] = x[i] / rhs;
            return Point(new_x);
        }

        template<size_t other_dim>
        inline Point operator+ (const Point<other_dim>& rhs) const {
            assert(dimension == other_dim);
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) new_x[i] = x[i] + rhs.x[i];
            return Point(new_x);
        }

        template<size_t other_dim>
        inline Point operator- (const Point<other_dim>& rhs) const {
            assert(dimension == other_dim);
            std::array<double, dimension> new_x;
            for(size_t i = 0; i < dimension; i ++) new_x[i] = x[i] - rhs.x[i];
            return Point(new_x);
        }


        template<size_t other_dim>
        inline double squared_distance(const Point& pt) const {
            assert(dimension == other_dim);
            double res(0.0);
            for(size_t i = 0; i < dimension; i ++) res += pow(x[i]-pt.x[i],2);
            return res;
        }

        template<size_t other_dim>
        inline double distance(const Point& pt) const {
            assert(dimension == other_dim);
            return sqrt(squared_distance(pt));
        }

        template<size_t other_dim>
        inline Point direction(const Point& pt) const {
            assert(dimension == other_dim);
            Point res(pt - *this);
            res /= distance(pt); return res;
        }

    };

typedef Point<2> Point2D;
typedef Point<3> Point3D;
}

template <size_t dimension>
void init_point_base(pybind11::module  m);

void init_point(pybind11::module  m);

#endif
/** @} */ // end of doxygen group
