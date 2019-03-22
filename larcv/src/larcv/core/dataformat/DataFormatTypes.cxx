#ifndef __LARCV_DATAFORMATTYPES_CXX
#define __LARCV_DATAFORMATTYPES_CXX

#include "DataFormatTypes.h"

#ifndef SWIG

namespace larcv{

  template<>
  H5::DataType get_datatype<int>()                {return H5::PredType::NATIVE_INT;}

  template<>
  H5::DataType get_datatype<float>()              {return H5::PredType::NATIVE_FLOAT;}

  template<>
  H5::DataType get_datatype<double>()             {return H5::PredType::NATIVE_DOUBLE;}

  template<>
  H5::DataType get_datatype<unsigned short>()     {return H5::PredType::NATIVE_USHORT;}

  template<>
  H5::DataType get_datatype<unsigned int>()       {return H5::PredType::NATIVE_UINT;}

  template<>
  H5::DataType get_datatype<short>()              {return H5::PredType::NATIVE_SHORT;}

  template<>
  H5::DataType get_datatype<unsigned long long>() {return H5::PredType::NATIVE_ULLONG;}
  
  template<>
  H5::DataType get_datatype<size_t>()             {return H5::PredType::NATIVE_HSIZE;}

  template<>
  H5::DataType get_datatype<ShapeType_t>()        {return get_datatype<int>();}

  template<>
  H5::DataType get_datatype<Extents_t>()          {
    H5::CompType datatype(sizeof(Extents_t));
    datatype.insertMember("first", offsetof(struct Extents_t, first), get_datatype<unsigned int>());
    datatype.insertMember("N",  offsetof(struct Extents_t, n),  get_datatype<unsigned int>());
    return datatype;
  }


}

#endif // swig


#endif
