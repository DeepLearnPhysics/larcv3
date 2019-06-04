#ifndef __LARCV3DATAFORMAT_DATAFORMATTYPES_CXX
#define __LARCV3DATAFORMAT_DATAFORMATTYPES_CXX

#include "larcv3/core/dataformat/DataFormatTypes.h"

#ifndef SWIG

namespace larcv3{



  template<>
  H5::DataType get_datatype<int>()                {
    H5::DataType _copied_type(H5::PredType::NATIVE_INT);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<bool>()               {
    H5::DataType _copied_type(H5::PredType::NATIVE_HBOOL);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<float>()              {
    H5::DataType _copied_type(H5::PredType::NATIVE_FLOAT);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<double>()             {
    H5::DataType _copied_type(H5::PredType::NATIVE_DOUBLE);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<unsigned short>()     {
    H5::DataType _copied_type(H5::PredType::NATIVE_USHORT);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<unsigned int>()       {
    H5::DataType _copied_type(H5::PredType::NATIVE_UINT);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<short>()              {
    H5::DataType _copied_type(H5::PredType::NATIVE_SHORT);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<unsigned long long>() {
    H5::DataType _copied_type(H5::PredType::NATIVE_ULLONG);
    return _copied_type;}
  
  template<>
  H5::DataType get_datatype<size_t>()             {
    H5::DataType _copied_type(H5::PredType::NATIVE_HSIZE);
    return _copied_type;}

  template<>
  H5::DataType get_datatype<ShapeType_t>()        {return get_datatype<int>();}

  template<>
  H5::DataType get_datatype<long>()               {
    H5::DataType _long_copy(H5::PredType::NATIVE_LONG);
    return _long_copy;
  }


  template<>
  H5::DataType get_datatype<Extents_t>()          {
    H5::CompType datatype(sizeof(Extents_t));
    datatype.insertMember("first", offsetof(struct Extents_t, first), get_datatype<unsigned int>());
    datatype.insertMember("N",     offsetof(struct Extents_t, n),     get_datatype<unsigned int>());
    return datatype;
  }

  template<>
  H5::DataType get_datatype<IDExtents_t>()          {
    H5::CompType datatype(sizeof(IDExtents_t));
    datatype.insertMember("first", offsetof(struct IDExtents_t, first), get_datatype<unsigned int>());
    datatype.insertMember("ID",    offsetof(struct IDExtents_t, id),    get_datatype<unsigned int>());
    datatype.insertMember("N",     offsetof(struct IDExtents_t, n),     get_datatype<unsigned int>());
    return datatype;
  }


}

#endif // swig


#endif
