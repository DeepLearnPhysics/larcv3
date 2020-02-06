cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


# general configuration for compilation:
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -g")


if(MPI)
    message (STATUS "MPI_CXX_COMPILER ${MPI_CXX_COMPILER}")
    message (STATUS "MPI_CXX_COMPILER ${MPI_C_COMPILER}")
    set(MPI_ASSUME_NO_BUILTIN_MPI True)
    find_package(MPI REQUIRED COMPONENTS C)
    include_directories(MPI_C_INCLUDE_DIRECTORIES)
    include_directories(MPI_CXX_INCLUDE_DIRECTORIES)
    add_definitions(-DLARCV_MPI)
    MESSAGE (STATUS "MPI_FOUND ${MPI_FOUND}")
    MESSAGE (STATUS "MPI_INCLUDE_PATH ${MPI_INCLUDE_PATH}")
    MESSAGE (STATUS "MPI_CXX_FOUND ${MPI_CXX_FOUND}")
    MESSAGE (STATUS "MPI_CXX_INCLUDE_DIRS ${MPI_CXX_INCLUDE_DIRS}")
    MESSAGE (STATUS "MPI_CXX_LIBRARIES ${MPI_CXX_LIBRARIES}")
    MESSAGE (STATUS "MPI_CXX_COMPILER ${MPI_CXX_COMPILER}")
    MESSAGE (STATUS "MPI_C_FOUND ${MPI_C_FOUND}")
    MESSAGE (STATUS "MPI_C_INCLUDE_DIRS ${MPI_C_INCLUDE_DIRS}")
    MESSAGE (STATUS "MPI_C_LIBRARIES ${MPI_C_LIBRARIES}")
    MESSAGE (STATUS "MPI_C_COMPILER ${MPI_C_COMPILER}")

endif()


# set(CMAKE_SKIP_RPATH TRUE)
project(larcv3)

set(CMAKE_PACKAGE_DIR "larcv")

set(PROJECT_SOURCE_DIR "./")


include_directories("./")

# IO needs HDF5:
find_package(HDF5 REQUIRED COMPONENTS C)
include_directories(${HDF5_INCLUDE_DIR})

# Many packages need python:
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

if(OPENMP)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
      add_definitions(-DLARCV_OPENMP)
  endif()
endif()

# This package needs numpy::

######################################
# This is from https://github.com/Eyescale/CMake/blob/master/FindNumPy.cmake
if(NOT PYTHON_EXECUTABLE)
  if(NumPy_FIND_QUIETLY)
    find_package(PythonInterp QUIET)
  else()
    find_package(PythonInterp)
    set(__numpy_out 1)
  endif()
endif()

if (PYTHON_EXECUTABLE)
  # Find out the include path
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "from __future__ import print_function\ntry: import numpy; print(numpy.get_include(), end='')\nexcept:pass\n"
            OUTPUT_VARIABLE __numpy_path)
  # And the version
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "from __future__ import print_function\ntry: import numpy; print(numpy.__version__, end='')\nexcept:pass\n"
    OUTPUT_VARIABLE __numpy_version)
elseif(__numpy_out)
  message(STATUS "Python executable not found.")
endif(PYTHON_EXECUTABLE)

find_path(PYTHON_NUMPY_INCLUDE_DIR numpy/arrayobject.h
  HINTS "${__numpy_path}" "${PYTHON_INCLUDE_PATH}" NO_DEFAULT_PATH)

if(PYTHON_NUMPY_INCLUDE_DIR)
  set(PYTHON_NUMPY_FOUND 1 CACHE INTERNAL "Python numpy found")
endif(PYTHON_NUMPY_INCLUDE_DIR)

message(${PYTHON_NUMPY_FOUND})
message(${PYTHON_NUMPY_INCLUDE_DIR})

# include(FindPackageHandleStandardArgs)
# find_package_handle_standard_args(NumPy REQUIRED_VARS PYTHON_NUMPY_INCLUDE_DIR
#                                         VERSION_VAR __numpy_version)
######################################

include_directories(${PYTHON_NUMPY_INCLUDE_DIR})


# # Define a function that handles generating python bindings for a library:
# macro(generate_python_bindings name)



# endmacro(generate_python_bindings)
if(CMAKE_PYTHON_BINDINGS)
  message("Adding pybind11")
  add_subdirectory(pybind11)
  include_directories(${PYBIND11_INCLUDE_DIR})
  message(${PYBIND11_INCLUDE_DIR})
else()
  add_definitions(-DLARCV_NO_PYBIND)
endif(CMAKE_PYTHON_BINDINGS)

add_subdirectory(larcv3)
