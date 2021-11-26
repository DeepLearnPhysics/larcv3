#ifndef __LARCV3PYUTIL_PYUTILS_H__
#define __LARCV3PYUTIL_PYUTILS_H__

struct _object;
typedef _object PyObject;

//#ifndef __CLING__
//#ifndef __CINT__
#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
//#include <numpy/ndarrayobject.h>
#include "numpy/arrayobject.h"
//#endif
//#endif

#include "larcv3/core/dataformat/Tensor.h"
#include "larcv3/core/dataformat/Voxel.h"
// #include "larcv3/core/dataformat/Voxel2D.h"
// #include "larcv3/core/dataformat/Voxel3DMeta.h"
// #include "larcv3/core/dataformat/EventVoxel3D.h"
namespace larcv3 {
/// Utility function: call one-time-only numpy module initialization (you don't
/// have to call)
inline int SetPyUtil() {
  static bool once = false;
  if (!once) {
    _import_array();
    once = true;
  }
  return 0;
}
///
template <class T>
PyObject* _as_ndarray(const std::vector<T>& data);
PyObject* as_ndarray(const std::vector< short              > &data);
PyObject* as_ndarray(const std::vector< unsigned short     > &data);
PyObject* as_ndarray(const std::vector< int                > &data);
PyObject* as_ndarray(const std::vector< unsigned int       > &data);
PyObject* as_ndarray(const std::vector< long long          > &data);
PyObject* as_ndarray(const std::vector< size_t             > &data);
PyObject* as_ndarray(const std::vector< unsigned long long > &data);
PyObject* as_ndarray(const std::vector< float              > &data);
PyObject* as_ndarray(const std::vector< double             > &data);
/// larcv3::Image2D to numpy array converter
PyObject* as_ndarray(const larcv3::Image2D &img);
PyObject* as_ndarray(const larcv3::Tensor1D &img);
PyObject* as_ndarray(const larcv3::Tensor3D &img);
PyObject* as_ndarray(const larcv3::Tensor4D &img);
// /// larcv3::Image2D to numpy array converter
// PyObject* as_caffe_ndarray(const Image2D &img);
// /// larcv3::VoxelSet to numpy (dense array) array converter
// PyObject* as_ndarray(const SparseTensor3D &data, bool clear_mem=false);
// /// larcv3::VoxelSet to numpy (dense array) array converter
// PyObject* as_ndarray(const SparseTensor2D &data, bool clear_mem=false);
// /// larcv3::VoxelSet to 2D numpy point cloud array (NUM_POINTS,1/3/4) converter
// void fill_3d_pcloud(const SparseTensor3D &data, PyObject* pyarray, PyObject* select=nullptr);
// /// larcv3::VoxelSet to 2D numpy point cloud array (NUM_POINTS,1/3/4) converter
// void fill_3d_voxels(const SparseTensor3D &data, PyObject* pyarray, PyObject* select=nullptr);
//  /// larcv3::VoxelSet to 2D numpy point cloud array (NUM_POINTS,1/2/3) converter
// void fill_2d_pcloud(const SparseTensor2D &data, PyObject* pyarray, PyObject* select=nullptr);
// /// larcv3::VoxelSet to 2D numpy point cloud array (NUM_POINTS,1/2/3) converter
// void fill_2d_voxels(const SparseTensor2D &data, PyObject* pyarray, PyObject* select=nullptr);
// /// copy array
template <class T>
void _copy_array(PyObject *arrayin, const std::vector<T> &cvec);
void copy_array(PyObject *arrayin, const std::vector< unsigned short > &cvec);
void copy_array(PyObject *arrayin, const std::vector< unsigned int   > &cvec);
void copy_array(PyObject *arrayin, const std::vector< short          > &cvec);
void copy_array(PyObject *arrayin, const std::vector< int            > &cvec);
void copy_array(PyObject *arrayin, const std::vector< long long      > &cvec);
void copy_array(PyObject *arrayin, const std::vector< float          > &cvec);
void copy_array(PyObject *arrayin, const std::vector< double         > &cvec);

// // /// Algorithm to extract three 1D arrays (x y and value) from 2D VoxelSet
// // void as_flat_arrays(const VoxelSet& tensor, const ImageMeta& meta,
// // 		    PyObject* x, PyObject* y, PyObject* value);

// // /// Algorithm to extract four 1D arrays (x y z and value) from 3D VoxelSet
// // void as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta,
// // 		    PyObject* x, PyObject* y, PyObject* z, PyObject* value);

// // /// Algorithm to extract four 1D arrays (index and value) from 3D VoxelSet
// // void as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta,
// // 		    PyObject* index, PyObject* value);

// //void copy_array(PyObject *arrayin, const std::vector<float> &cvec);
// // void copy_array(PyObject *arrayin);//, const std::vector<float>& vec);

larcv3::Image2D as_image2d_meta(PyObject *, larcv3::ImageMeta2D meta);

larcv3::Image2D as_image2d(PyObject *);

larcv3::Tensor1D as_tensor1d(PyObject *);

larcv3::Tensor3D as_tensor3d(PyObject *);

larcv3::Tensor4D as_tensor4d(PyObject *);

larcv3::VoxelSet as_voxelset(PyObject * values, PyObject * indexes);

// VoxelSet as_tensor3d(PyObject *, float min_threshold=0);
 
// VoxelSet as_tensor3d(PyObject* pyarray, const Voxel3DMeta& meta, float min_threshold=0);

// VoxelSet as_tensor3d(PyObject* pos_array, PyObject* val_array, const Voxel3DMeta& meta, float min_threshold=0);

// VoxelSet as_tensor3d(const SparseTensor3D& vs, PyObject* val_array, float min_threshold=0);

// inline VoxelSet as_tensor3d(const EventSparseTensor3D& vs, PyObject* val_array, float min_threshold=0)
// { return as_tensor3d( (const SparseTensor3D)vs, val_array, min_threshold); }

// // allows one to avoid some loops in python
// void fill_img_col(Image2D &img, std::vector<short> &adcs, const int col,const float pedestal = 0.0);
//                   //const int timedownsampling, const float pedestal = 0.0);

template <class T> int ctype_to_numpy();
template<> int ctype_to_numpy<short>();
template<> int ctype_to_numpy<unsigned short>();
template<> int ctype_to_numpy<int>();
template<> int ctype_to_numpy<unsigned int>();
template<> int ctype_to_numpy<long long>();
template<> int ctype_to_numpy<unsigned long long>();
template<> int ctype_to_numpy<float>();
template<> int ctype_to_numpy<double>();
template <class T> PyObject* numpy_array(std::vector<size_t> dims);
}

#endif
