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

namespace larcv3 {

    template<size_t dimension>
    class Point {
    public:
        Point(){};
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

    };

  // /**
  //    \class Point2D
  //    Simple 2D point struct (unit of "x" and "y" are not defined here and app specific)
  // */
  // class Point2D {
  // public:
  //   Point2D(double xv=0, double yv=0) : x(xv), y(yv) {}
  //   ~Point2D() {}

  //   Point2D(const Point2D& pt) : x(pt.x), y(pt.y) {};

  //   double x, y;
    
  //   inline bool operator== (const Point2D& rhs) const
  //   { return (x == rhs.x && y == rhs.y); }
  //   inline bool operator!= (const Point2D& rhs) const
  //   { return !(rhs == (*this)); }

  //   inline Point2D& operator*= (const double rhs)
  //   { x *= rhs; y *= rhs; return (*this); }
  //   inline Point2D& operator/= (const double rhs)
  //   { x /= rhs; y /= rhs; return (*this); }
  //   inline Point2D& operator+= (const Point2D& rhs)
  //   { x += rhs.x; y += rhs.y; return (*this); }
  //   inline Point2D& operator-= (const Point2D& rhs)
  //   { x -= rhs.x; y -= rhs.y; return (*this); }

  //   inline Point2D operator* (const double rhs) const
  //   { return Point2D(x*rhs,y*rhs); }
  //   inline Point2D operator/ (const double rhs) const
  //   { return Point2D(x/rhs,y/rhs); }
  //   inline Point2D operator+ (const Point2D& rhs) const
  //   { return Point2D(x+rhs.x,y+rhs.y); }
  //   inline Point2D operator- (const Point2D& rhs) const
  //   { return Point2D(x-rhs.x,y-rhs.y); }

  //   inline double squared_distance(const Point2D& pt) const
  //   { return pow(x-pt.x,2)+pow(y-pt.y,2); }
  //   inline double distance(const Point2D& pt) const
  //   { return sqrt(squared_distance(pt)); }
  //   inline Point2D direction(const Point2D& pt) const
  //   { Point2D res(pt.x - x, pt.y - y); res /= distance(pt); return res; }

  // };

  // *
  //    \class Point3D
  //    Simple 3D point struct (unit of "x", "y" and "z" are not defined here and app specific)
  
  // class Point3D {
  // public:
  //   Point3D(double xv=0, double yv=0, double zv=0) : x(xv), y(yv), z(zv) {}
  //   ~Point3D() {}

  //   Point3D(const Point3D& pt) : x(pt.x), y(pt.y), z(pt.z) {}

  //   double x, y, z;
    
  //   inline bool operator== (const Point3D& rhs) const
  //   { return (x == rhs.x && y == rhs.y && z == rhs.z); }
  //   inline bool operator!= (const Point3D& rhs) const
  //   { return !(rhs == (*this)); }

  //   inline Point3D& operator/= (const double rhs)
  //   { x /= rhs; y /= rhs; z /= rhs; return (*this); }
  //   inline Point3D& operator*= (const double rhs)
  //   { x *= rhs; y *= rhs; z *= rhs; return (*this); }
  //   inline Point3D& operator+= (const Point3D& rhs)
  //   { x += rhs.x; y += rhs.y; z += rhs.z; return (*this); }
  //   inline Point3D& operator-= (const Point3D& rhs)
  //   { x -= rhs.x; y -= rhs.y; z -= rhs.z; return (*this); }

  //   inline Point3D operator/ (const double rhs) const
  //   { return Point3D(x/rhs,y/rhs,z/rhs); }
  //   inline Point3D operator* (const double rhs) const
  //   { return Point3D(x*rhs,y*rhs,z*rhs); }
  //   inline Point3D operator+ (const Point3D& rhs) const
  //   { return Point3D(x+rhs.x,y+rhs.y,z+rhs.z); }
  //   inline Point3D operator- (const Point3D& rhs) const
  //   { return Point3D(x-rhs.x,y-rhs.y,z-rhs.z); }

  //   inline double squared_distance(const Point3D& pt) const
  //   { return pow(x-pt.x,2)+pow(y-pt.y,2)+pow(z-pt.z,2); }
  //   inline double distance(const Point3D& pt) const
  //   { return sqrt(squared_distance(pt)); }
  //   inline Point3D direction(const Point3D& pt) const
  //   { Point3D res(pt.x - x, pt.y - y, pt.z - z); res /= distance(pt); return res; }

  // };


typedef Point<2> Point2D;
typedef Point<3> Point3D;


}
#endif
/** @} */ // end of doxygen group 

