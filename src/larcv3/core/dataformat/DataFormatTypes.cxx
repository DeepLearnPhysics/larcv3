#ifndef __LARCV3DATAFORMAT_DATAFORMATTYPES_CXX
#define __LARCV3DATAFORMAT_DATAFORMATTYPES_CXX

#include "larcv3/core/dataformat/DataFormatTypes.h"

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
    datatype = H5Tcreate (H5T_COMPOUND, sizeof (Extents_t));
    H5Tinsert (datatype, "first",
               HOFFSET (Extents_t, first),
               larcv3::get_datatype<unsigned long long int>());
    H5Tinsert (datatype, "N",
               HOFFSET (Extents_t, n),     
               larcv3::get_datatype<unsigned int>());
    return datatype;

  }

  template<>
  hid_t get_datatype<IDExtents_t>()          {
    hid_t datatype;
    datatype = H5Tcreate (H5T_COMPOUND, sizeof (IDExtents_t));
    H5Tinsert (datatype, "first",
               HOFFSET (IDExtents_t, first),
               larcv3::get_datatype<unsigned long long int>());
    H5Tinsert (datatype, "N",
               HOFFSET (IDExtents_t, n),
               larcv3::get_datatype<unsigned int>());
    H5Tinsert (datatype, "ID",
               HOFFSET (IDExtents_t, id),
               larcv3::get_datatype<unsigned int>());
    return datatype;
  }


  // Wrapper functions and enumerations to make binding easier:
  template<> std::string as_string<float>() {return "Float";}
  template<> std::string as_string<int>()   {return "Int";}
  template<> std::string as_string<double>(){return "Double";}
  template<> std::string as_string<short>() {return "Short";}

}

void init_dataformattypes(pybind11::module m){
  pybind11::enum_<larcv3::DistanceUnit_t> distanceunit_t(m,"DistanceUnit_t");
  distanceunit_t.value("kUnitUnknown",  larcv3::kUnitUnknown);
  distanceunit_t.value("kUnitCM",       larcv3::kUnitCM);
  distanceunit_t.value("kUnitWireTime", larcv3::kUnitWireTime);
  distanceunit_t.export_values();

  pybind11::class_<larcv3::Extents_t>   extents_t(m, "Extents_t");
  extents_t.def(pybind11::init<>());
  extents_t.def_readwrite("first", &larcv3::Extents_t::first);
  extents_t.def_readwrite("n",     &larcv3::Extents_t::n);

  pybind11::class_<larcv3::IDExtents_t> idextents_t(m, "IDExtents_t");
  idextents_t.def(pybind11::init<>());
  idextents_t.def_readwrite("first", &larcv3::IDExtents_t::first);
  idextents_t.def_readwrite("n",     &larcv3::IDExtents_t::n);
  idextents_t.def_readwrite("id",    &larcv3::IDExtents_t::id);


  m.attr("kINVALID_INDEX")        = larcv3::kINVALID_INDEX;
  m.attr("kINVALID_INSTANCEID")   = larcv3::kINVALID_INSTANCEID;
  m.attr("kINVALID_PROJECTIONID") = larcv3::kINVALID_PROJECTIONID;
  m.attr("kINVALID_VOXELID")      = larcv3::kINVALID_VOXELID;
  m.attr("kINVALID_PRODUCER")     = larcv3::kINVALID_PRODUCER;
  
  pybind11::enum_<larcv3::PoolType_t> pooltype_t(m,"PoolType_t");
  pooltype_t.value("kPoolSum",      larcv3::PoolType_t::kPoolSum);
  pooltype_t.value("kPoolAverage",  larcv3::PoolType_t::kPoolAverage);
  pooltype_t.value("kPoolMax",      larcv3::PoolType_t::kPoolMax);
  pooltype_t.export_values();
  // struct Extents_t{
  //   unsigned long long int first;
  //   unsigned int n;
  // };

  // struct IDExtents_t{
  //   unsigned long long int first;
  //   unsigned int n;
  //   unsigned int id;
  // };

}


#endif
