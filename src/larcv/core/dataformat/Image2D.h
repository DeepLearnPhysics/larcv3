/**
 * \file Image2D.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for an image2D data holder larcv::Image2D
 *
 * @author tmw, kazu, cadams
 */

/** \addtogroup core_DataFormat

    @{*/

#ifndef __LARCV_IMAGE_H__
#define __LARCV_IMAGE_H__

#include <vector>
#include <cstdlib>
#include "ImageMeta.h"

namespace larcv {

  /**
    \TODO Need to  work on implementations for overlay, resize, crop
    \class Image2D
    Meant to be a storage class for an image2D into a ROOT file. Ultimately data is 1D array.
  */
  class Image2D {
    
  public:
    /// ctor by dimensions
    Image2D(const std::vector<size_t> & dims);
    /// ctor from ImageMeta
    Image2D(const ImageMeta2D&);
    /// ctor from ImageMeta and 1D array data
    Image2D(const ImageMeta2D&, const std::vector<float>&);
    /// copy ctor
    Image2D(const Image2D&);
    

    /// dtor
    virtual ~Image2D(){}

    /// Reset contents w/ new larcv::ImageMeta
    void reset(const ImageMeta2D&);
    /// Various modes used to combine pixels
    enum CompressionModes_t { kSum, kAverage, kMaxPool, kOverWrite};

    /// Return image2D index ID number (should be unique within larcv::EventImage2D)
    ImageIndex_t index() const { return _id; }
    /// Index setter
    void index(ImageIndex_t n) { _id = n; }
    /// Size of data, equivalent of # rows x # columns x ...
    size_t size() const { return _img.size(); }
    /// Specific pixel value getter
    float pixel(const std::vector<size_t> & coords) const;
    /// Specific pixel value getter
    float pixel(size_t index) const;
    /// larcv::ImageMeta const reference getter
    const ImageMeta2D& meta() const { return _meta; }

    /// Mem-copy: insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(const std::vector<size_t> & coords, const float* src, size_t num_pixel);
    /// Mem-copy: insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(const std::vector<size_t> & coords, const std::vector<float>& src, size_t num_pixel=0);
    /// Fake mem-copy (loop element-wise copy): insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(const std::vector<size_t> & coords, const short* src, size_t num_pixel);
    /// Fake mem-copy (loop element-wise copy): insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(const std::vector<size_t> & coords, const std::vector<short>& src, size_t num_pixel=0);
    /// Same as copy, but perform in reverse direction of rows (useful when src is not in the same order)
    void reverse_copy(const std::vector<size_t> & coords, const std::vector<float>& src, size_t nskip=0, size_t num_pixel=0);
    /// Same as copy, but perform in reverse direction of rows (useful when src is not in the same order)
    void reverse_copy(const std::vector<size_t> & coords, const std::vector<short>& src, size_t nskip=0, size_t num_pixel=0);
    // /// Crop specified region via crop_meta to generate a new larcv::Image2D
    // Image2D crop(const ImageMeta& crop_meta) const;
    /// 1D const reference array getter
    const std::vector<float>& as_vector() const 
    { return _img; }
    // /// Re-size the 1D data array w/ updated # rows and # columns
    // void resize( const std::vector<size_t> &  counts, float fillval=0.0 );
    /// Set pixel value via row/col specification
    void set_pixel( const std::vector<size_t> & coords, float value );
    /// Set pixel value via index specification
    void set_pixel( size_t index, float value );
    /// Paint all pixels with a specified value
    void paint(float value);
    /// Apply threshold: pixels lower than "thres" are all overwritten by lower_overwrite value
    void threshold(float thresh, bool lower);
    /// Apply threshold: make all pixels to take only 2 values, lower_overwrite or upper_overwrite 
    void binarize(float thresh, float lower_overwrite, float upper_overwrite);
    /// Clear data contents
    void clear_data();

    // /// Overlay with another Image2D: overlapped pixel region is merged
    // void overlay(const Image2D&, CompressionModes_t mode=kSum);
    /// Move data contents out
    std::vector<float>&& move();
    /// Move data contents in
    void move(std::vector<float>&&);

    inline Image2D& operator+=(const float val)
    { for(auto& v : _img) v+= val; return (*this);}
    inline Image2D operator+(const float val) const
    { Image2D res = (*this); res+=val; return res; }
    inline Image2D& operator-=(const float val)
    { for(auto& v : _img) v-= val; return (*this);}
    inline Image2D operator-(const float val) const
    { Image2D res = (*this); res-=val; return res; }
    inline Image2D& operator*=(const float val)
    { for(auto& v : _img) v*= val; return (*this);}
    inline Image2D operator*(const float val) const
    { Image2D res = (*this); res*=val; return res; }
    inline Image2D& operator/=(const float val)
    { for(auto& v : _img) v/= val; return (*this);}
    inline Image2D operator/(const float val) const
    { Image2D res = (*this); res/=val; return res; }

    Image2D& operator +=(const std::vector<float>& rhs);
    Image2D& operator -=(const std::vector<float>& rhs);
    Image2D& operator +=(const larcv::Image2D& rhs);



    /// Element-wise pixel value multiplication
    void eltwise( const Image2D& rhs );
    /// Element-wise multiplication w/ 1D array data
    void eltwise(const std::vector<float>& arr,bool allow_longer=false);
    
    // The following functions were deprecated for larcv3.
    // Implementations are here in the source code until a later cleanup, in case they
    // need to be un deprecated:

    /*
    /// attribute move ctor
    Image2D(ImageMeta&&, std::vector<float>&&);

    /// Move origin position
    void reset_origin(double x, double y) {_meta.reset_origin(x,y);}
    
    /// Compress image2D data and returns compressed data 1D array
    std::vector<float> copy_compress(size_t row_count, size_t col_count, CompressionModes_t mode=kSum) const;

    /// Paint a row of pixels with a specified value
    void paint_row( int row, float value );
    /// Paint a column of pixels with a specified value
    void paint_col( int col, float value );
    /// Call copy_compress internally and set itself to the result
    void compress(size_t row_count, size_t col_count, CompressionModes_t mode=kSum);
    
    // Matrix Multiplication
    /// Matrix multiplicaition
    Image2D multiRHS( const Image2D& rhs ) const; 


    // /// uniry operator for matrix multiplicaition
    // Image2D& operator*=( const Image2D& rhs );
    // /// binary operator for matrix multiplication
    // Image2D operator*(const Image2D& rhs) const;

    */
  private:
    std::vector<float> _img;
    ImageIndex_t _id;
    ImageMeta2D _meta ;
    void clear();
  };

}

#endif
/** @} */ // end of doxygen group 
