cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


# general configuration for compilation:
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

set(CMAKE_PACKAGE_DIR "larcv")

# # Control rpath handling:
# set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)



project(larcv)

include_directories(larcv)

# IO needs HDF5:
find_package(HDF5 REQUIRED COMPONENTS C CXX)
include_directories(${HDF5_INCLUDE_DIR})

# Many pacakges need python:
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})



add_subdirectory(larcv)


# add_library(larcv SHARED 
#     $<TARGET_OBJECTS:base>
#     $<TARGET_OBJECTS:dataformat>
#     $<TARGET_OBJECTS:processor>
#     $<TARGET_OBJECTS:datagenerator>
#     )


# target_link_libraries(larcv PUBLIC ${HDF5_CXX_LIBRARIES}) 

# install(TARGETS larcv
#     LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
#     ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
#     RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
#     INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
#     )



