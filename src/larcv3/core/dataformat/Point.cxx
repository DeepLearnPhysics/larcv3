#ifndef __LARCV3DATAFORMAT_POINT_CXX
#define __LARCV3DATAFORMAT_POINT_CXX

#include <pybind11/stl.h>


#include "larcv3/core/dataformat/Point.h"


namespace larcv3{
template class Point<2>;
template class Point<3>;
}

template<size_t dimension>
void init_point_base(pybind11::module m){
    std::string classname = "Point" + std::to_string(dimension) + "D";
    pybind11::class_<larcv3::Point<dimension>>(m, classname.c_str())
        .def(pybind11::init<>())
        .def(pybind11::init<std::array<double, dimension> > ())
        // .def("logger", &larcv3::Point<dimension>::logger)
        // .def("set_verbosity", &larcv3::Point<dimension>::set_verbosity)
        // .def("name", &larcv3::Point<dimension>::name)
        ;

/*

        Point() : x() { };
        Point(double * xv){
            for(size_t i = 0; i < dimension; i ++) x[i] = xv[i];
        }
        ~Point() {}

        template<size_t other_dim>
        Point(const Point<other_dim>& pt){
            assert(dimension == other_dim);
            for(size_t i = 0; i < dimension; i ++) x[i] = pt[i];
        };

        double x[dimension];

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
            double new_x[dimension];
            for(size_t i = 0; i < dimension; i ++) new_x[i] = rhs * x[i];
            return Point(new_x);
        }
        inline Point operator/ (const double rhs) const {
            double new_x[dimension];
            for(size_t i = 0; i < dimension; i ++) new_x[i] = x[i] / rhs;
            return Point(new_x);
        }

        template<size_t other_dim>
        inline Point operator+ (const Point<other_dim>& rhs) const {
            assert(dimension == other_dim);
            double new_x[dimension];
            for(size_t i = 0; i < dimension; i ++) new_x[i] = x[i] + rhs.x[i];
            return Point(new_x);
        }

        template<size_t other_dim>
        inline Point operator- (const Point<other_dim>& rhs) const {
            assert(dimension == other_dim);
            double new_x[dimension];
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



*/

}

void init_point(pybind11::module m){
    init_point_base<2>(m);
    init_point_base<3>(m);
}

#endif
