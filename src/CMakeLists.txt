cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


# general configuration for compilation:
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")


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





