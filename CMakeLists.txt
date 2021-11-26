cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
###############################################################################
# set(CMAKE_SKIP_RPATH TRUE)
# set the project name to larcv3
project(larcv3)
set(CMAKE_PACKAGE_DIR "larcv")
set(PROJECT_SOURCE_DIR "src/")
set(PROJECT_INCLUDE_DIR "include/")
###############################################################################
# general configuration for compilation:
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17 CACHE PATH "CXX standard for this build")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O3 -g")

enable_language(C)
###############################################################################
# We set a larcv compiler definition to build all of the python code
# This is obviously not set if you are including headers, which protects
# the need to install pybind11 outside of larcv (since it's header only,
# there is no need to link to it.)
add_definitions(-DLARCV_INTERNAL)
###############################################################################
# set up headers
file(GLOB headers 
    ${PROJECT_INCLUDE_DIR}larcv3/base/*.hh
    ${PROJECT_INCLUDE_DIR}larcv3/dataformat/*.hh
    ${PROJECT_INCLUDE_DIR}larcv3/processor/*.hh
    ${PROJECT_INCLUDE_DIR}larcv3/pyutil/*.hh
    #${PROJECT_INCLUDE_DIR}/app/filter/*.hh
    #${PROJECT_INCLUDE_DIR}/app/imagemod/*.hh
    #${PROJECT_INCLUDE_DIR}/app/merger/*.hh
    #${PROJECT_INCLUDE_DIR}/app/next_image_mod/*.hh
    ${PROJECT_INCLUDE_DIR}/app/queueio/*.hh
    #${PROJECT_INCLUDE_DIR}/app/sbnd_imagemod/*.hh
)
file(GLOB sources 
    ${PROJECT_SOURCE_DIR}/core/base/*.cxx
    ${PROJECT_SOURCE_DIR}/core/dataformat/*.cxx
    ${PROJECT_SOURCE_DIR}/core/processor/*.cxx
    ${PROJECT_SOURCE_DIR}/core/pyutil/*.cxx
    #${PROJECT_SOURCE_DIR}/app/filter/*.cxx
    #${PROJECT_SOURCE_DIR}/app/imagemod/*.cxx
    #${PROJECT_SOURCE_DIR}/app/merger/*.cxx
    #${PROJECT_SOURCE_DIR}/app/next_image_mod/*.cxx
    ${PROJECT_SOURCE_DIR}/app/queueio/*.cxx
    #${PROJECT_SOURCE_DIR}/app/sbnd_imagemod/*.cxx
)
###############################################################################
include_directories(
    "include"
    "include/larcv3/"
    "include/larcv3/core/"
    "include/larcv3/core/base/"
    "include/larcv3/core/dataformat/"
    "include/larcv3/core/processor/"
    "include/larcv3/core/pyutil/"
    "include/larcv3/app/"
    "include/larcv3/app/filter/"
    "include/larcv3/app/imagemod/"
    "include/larcv3/app/merger/"
    "include/larcv3/app/next_image_mod/"
    "include/larcv3/app/queueio/"
    "include/larcv3/app/sbnd_imagemod/"
    "include/pybind11/include/"
)
###############################################################################
# option for using MPI
option(USE_MPI "Compile with MPI" OFF)
if(USE_MPI)
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
###############################################################################
# option for using OpenMP
option(USE_OPENMP "Compile with OpenMP" OFF)
if(USE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        add_definitions(-DLARCV_OPENMP)
    endif()
endif()
###############################################################################
# Many packages need python:
message("Searching for python libraries...")
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
######################################
# section for python/numpy/pybind11
# This is from https://github.com/Eyescale/CMake/blob/master/FindNumPy.cmake
option(PYTHON_EXECUTABLE "Creates python executable" ON)
message("Adding python")
if(NOT PYTHON_EXECUTABLE)
    message(STATUS "No python executable, searching for numpy quietly...")
    if(NumPy_FIND_QUIETLY)
        find_package(PythonInterp QUIET)
    else()
        find_package(PythonInterp)
        set(__numpy_out 1)
    endif()
endif()
if (PYTHON_EXECUTABLE)
    message(STATUS "Python executable: ${PYTHON_EXECUTABLE}")
    message(STATUS "Checking for Numpy path and version...")
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
    message(STATUS "ERROR: Python executable not found.")
endif(PYTHON_EXECUTABLE)
###############################################################################
# IO needs HDF5:
option(USE_HDF5 "Compile LArCV to use HDF5 library" ON)
if(USE_HDF5)
    add_definitions(-DUSE_HDF5)
    find_package(HDF5 REQUIRED)
    include_directories(${HDF5_INCLUDE_DIR})
    message(STATUS "HDF5 library: ${HDF5_INCLUDE_DIR}")
endif()
option(USE_HDF5_PARALLEL "Whether to use parallelization with hdf5" OFF)
if(USE_HDF5_PARALLEL)
    if(HDF5_IS_PARALLEL)
        set(HDF5_PREFER_PARALLEL TRUE)
        message("Using hdf5 parallel: " ${HDF5_IS_PARALLEL})
    else()
        message(STATUS "ERROR: Can't use hdf5 parallel!")
    endif()
endif()
###############################################################################
# numpy library
message("Adding numpy")
find_path(PYTHON_NUMPY_INCLUDE_DIR numpy/arrayobject.h
    HINTS "${__numpy_path}" "${PYTHON_INCLUDE_PATH}" NO_DEFAULT_PATH)
if(PYTHON_NUMPY_INCLUDE_DIR)
    set(PYTHON_NUMPY_FOUND 1 CACHE INTERNAL "Numpy library found.")
    message(STATUS "Numpy library found.")
    message(STATUS "Numpy directory: ${PYTHON_NUMPY_INCLUDE_DIR}")
else()
    message(STATUS "ERROR: Numpy library not found!")
endif(PYTHON_NUMPY_INCLUDE_DIR)
include_directories(${PYTHON_NUMPY_INCLUDE_DIR})
###############################################################################
if(CMAKE_PYTHON_BINDINGS)
    message("Adding pybind11")
    add_subdirectory(include/pybind11)
    include_directories(${PYBIND11_INCLUDE_DIR})
    message(STATUS "Pybind11 location: ${PYBIND11_INCLUDE_DIR}")
else()
    add_definitions(-DLARCV_NO_PYBIND)
endif(CMAKE_PYTHON_BINDINGS)
###############################################################################
# add the src directory CMakeLists.txt
# and recursively go through it.  This must be done before
# the linked library is defined below.
add_subdirectory(src)
###############################################################################
add_library(larcv3 SHARED
    # $<TARGET_OBJECTS:base>
    # $<TARGET_OBJECTS:dataformat>
    # $<TARGET_OBJECTS:processor>
    # $<TARGET_OBJECTS:filter>
    # $<TARGET_OBJECTS:queueio>
    # $<TARGET_OBJECTS:imagemod>
    # $<TARGET_OBJECTS:sbnd_imagemod>
    ${sources}
    ${headers}
)
###############################################################################
if (OpenMP_CXX_FOUND)
    message("Linking against openmp")
    message("OpenMP_CXX_LIBRARY: " ${OpenMP_CXX_LIBRARIES})
    message("OpenMP_C_LIBRARY: " ${OpenMP_C_LIBRARIES})
    target_link_libraries(larcv3 ${OpenMP_CXX_LIBRARIES} ${OpenMP_C_LIBRARIES})
endif()
###############################################################################
if (MPI_FOUND)
    message("Linking against MPI")
    message("OpenMP_CXX_LIBRARY: " ${MPI_CXX_LIBRARIES})
    message("OpenMP_C_LIBRARY: " ${MPI_C_LIBRARIES})
    target_link_libraries(larcv3 ${MPI_CXX_LIBRARIES} ${MPI_C_LIBRARIES})
endif()
###############################################################################
# Link against python:
target_link_libraries(larcv3 ${PYTHON_LIBRARIES})
target_link_libraries(larcv3 ${HDF5_LIBRARIES})
###############################################################################
if(CMAKE_PYTHON_BINDINGS)
    message("Building python bindings...")
    pybind11_add_module(pylarcv SHARED src/larcv.cc)
    # Add a shared library
    set_target_properties(pylarcv PROPERTIES SKIP_BUILD_RPATH FALSE)
    # If building with setuptools, CMake will not be performing the install
    set_target_properties(pylarcv PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    target_link_libraries(pylarcv PRIVATE larcv3)
    if(UNIX AND NOT APPLE)
        set_target_properties(pylarcv PROPERTIES INSTALL_RPATH "$ORIGIN/lib/")
    elseif(APPLE)
        set_target_properties(pylarcv PROPERTIES INSTALL_RPATH "@loader_path/lib/")
    endif()
    set_target_properties(pylarcv PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(CMAKE_PYTHON_BINDINGS)
###############################################################################
if (MPI)
    target_link_libraries(larcv3 ${MPI_LIBRARIES})
endif()
###############################################################################
# Install larvc shared library
install(TARGETS larcv3
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
    INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
)
###############################################################################
if(CMAKE_PYTHON_BINDINGS)
    # Install larvc wrapper library
    install(TARGETS pylarcv
        LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}
        ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}
        RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
        INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
    )
endif()
###############################################################################