#ifndef __LARCV3DATAFORMAT_DATAFORMATTYPES_CXX
#define __LARCV3DATAFORMAT_DATAFORMATTYPES_CXX

#include "larcv3/core/dataformat/DataFormatTypes.h"

#ifndef SWIG

namespace larcv3{



  template<>
  hid_t get_datatype<int>()                {
    hid_t _copied_type(H5T_NATIVE_INT);
    return _copied_type;}

  template<>
  hid_t get_datatype<bool>()               {
    hid_t _copied_type(H5T_NATIVE_HBOOL);
    return _copied_type;}

  template<>
  hid_t get_datatype<float>()              {
    hid_t _copied_type(H5T_NATIVE_FLOAT);
    return _copied_type;}

  template<>
  hid_t get_datatype<double>()             {
    hid_t _copied_type(H5T_NATIVE_DOUBLE);
    return _copied_type;}

  template<>
  hid_t get_datatype<unsigned short>()     {
    hid_t _copied_type(H5T_NATIVE_USHORT);
    return _copied_type;}

  template<>
  hid_t get_datatype<unsigned int>()       {
    hid_t _copied_type(H5T_NATIVE_UINT);
    return _copied_type;}

  template<>
  hid_t get_datatype<short>()              {
    hid_t _copied_type(H5T_NATIVE_SHORT);
    return _copied_type;}

  template<>
  hid_t get_datatype<unsigned long long>() {
    hid_t _copied_type(H5T_NATIVE_ULLONG);
    return _copied_type;}
  
  template<>
  hid_t get_datatype<size_t>()             {
    hid_t _copied_type(H5T_NATIVE_HSIZE);
    return _copied_type;}

  template<>
  hid_t get_datatype<ShapeType_t>()        {return get_datatype<int>();}

  template<>
  hid_t get_datatype<long>()               {
    hid_t _long_copy(H5T_NATIVE_LONG);
    return _long_copy;
  }


  template<>
  hid_t get_datatype<Extents_t>()          {

    hid_t datatype;
    herr_t status;
    datatype = H5Tcreate (H5T_COMPOUND, sizeof (Extents_t));
    status = H5Tinsert (datatype, "first",
                HOFFSET (Extents_t, first), larcv3::get_datatype<unsigned int>());
    status = H5Tinsert (datatype, "N", 
                HOFFSET (Extents_t, n),     larcv3::get_datatype<unsigned int>());
    return datatype;

  }

  template<>
  hid_t get_datatype<IDExtents_t>()          {
    hid_t datatype;
    herr_t status;
    datatype = H5Tcreate (H5T_COMPOUND, sizeof (IDExtents_t));
    status = H5Tinsert (datatype, "first",
                HOFFSET (IDExtents_t, first), larcv3::get_datatype<unsigned int>());
    status = H5Tinsert (datatype, "N", 
                HOFFSET (IDExtents_t, n),     larcv3::get_datatype<unsigned int>());
    status = H5Tinsert (datatype, "ID",    
                HOFFSET (IDExtents_t, id),    larcv3::get_datatype<unsigned int>());
    return datatype;
  }


}

#endif // swig


#endif
