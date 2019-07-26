cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


# general configuration for compilation:
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -g")

if(MPI)
    # set(MPI_ASSUME_NO_BUILTIN_MPI True)
    find_package(MPI COMPONENTS C CXX)
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
find_package(HDF5 REQUIRED COMPONENTS C CXX)
include_directories(${HDF5_INCLUDE_DIR})

# Many packages need python:
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})



# # Define a function that handles generating python bindings for a library:
# macro(generate_python_bindings name)    
    
    

# endmacro(generate_python_bindings)



add_subdirectory(larcv3)





