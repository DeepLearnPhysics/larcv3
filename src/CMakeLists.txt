cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


# general configuration for compilation:
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -g")

enable_language(C CXX)

# We set a larcv compiler definition to build all of the python code
# This is obviously not set if you are including headers, which protects
# the need to install pybind11 outside of larcv (since it's header only,
# there is no need to link to it.)
add_definitions(-DLARCV_INTERNAL)


if(MPI)
    set(MPI_ASSUME_NO_BUILTIN_MPI True)
    find_package(MPI REQUIRED COMPONENTS C CXX)
    include_directories(MPI_C_INCLUDE_DIRECTORIES)
    include_directories(MPI_CXX_INCLUDE_DIRECTORIES)
    add_definitions(-DLARCV_MPI)
endif()


# set(CMAKE_SKIP_RPATH TRUE)
project(larcv3)

set(CMAKE_PACKAGE_DIR "larcv")



include_directories("./")

# IO needs HDF5:
find_package(HDF5 REQUIRED)
include_directories(${HDF5_INCLUDE_DIR})

if(OPENMP)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
      MESSAGE(STATUS "OpenMP Include directories ${OpenMP_CXX_INCLUDE_DIRS}")
      include_directories(${OpenMP_C_INCLUDE_DIRS})
      include_directories(${OpenMP_CXX_INCLUDE_DIRS})
      add_definitions(-DLARCV_OPENMP)
  endif()
endif()

# This package needs numpy::




# Add the json library
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)
message("NLOHMANN_JSON_INCLUDE_INSTALL_DIR: ${NLOHMANN_JSON_INCLUDE_INSTALL_DIR}")
include_directories(json/include)


# endmacro(generate_python_bindings)
if(CMAKE_PYTHON_BINDINGS)
  
  # Many packages need python:
  find_package(Python ${CMAKE_PYVERSION} EXACT REQUIRED COMPONENTS  Interpreter Development)
  include_directories(${Python_INCLUDE_DIRS})

  message("Adding pybind11")
  add_subdirectory(pybind11)
  include_directories(${PYBIND11_INCLUDE_DIR})

  set(pybind11_DIR pybind11)

  # GIves bindings between json and python
  add_subdirectory(pybind11_json)
  include_directories(pybind11_json/include)

else()
  add_definitions(-DLARCV_NO_PYBIND)
endif(CMAKE_PYTHON_BINDINGS)



add_subdirectory(larcv3)

if(DOCS)
  set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/docs" ${CMAKE_MODULE_PATH})
  add_subdirectory(docs)
endif()
