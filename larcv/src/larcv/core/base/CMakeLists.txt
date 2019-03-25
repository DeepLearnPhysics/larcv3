set(name base)


project(${name})



# Get all the source files:
file(GLOB SOURCES *.cxx)

# Add a shared library
add_library(${name} SHARED ${SOURCES})


# Make sure swig is available:
find_package(SWIG REQUIRED COMPONENTS C CXX)
include(${SWIG_USE_FILE})

# Find the swig source file:
file(GLOB SWIGSOURCES "*.i" )
# Designate that this is a C++ file
set_source_files_properties(${SWIGSOURCES} PROPERTIES CPLUSPLUS ON)

# Add as a module ('.so' extension on mac)
swig_add_library(${name} 
    TYPE MODULE
    LANGUAGE python 
    SOURCES ${SWIGSOURCES}
    )

# Link to python and core shared library
swig_link_libraries(${name} ${PYTHON_LIBRARIES} ${name})
# swig_add_library(larcv)

# install( TARGETS ${SWIG_MODULE_${name}_REAL_NAME}
#     LIBRARY DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/larcv )

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.py 
#     DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/larcv)

# Install swig module
install( TARGETS ${SWIG_MODULE_${name}_REAL_NAME}
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR} )

# Install swig python module
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.py 
    DESTINATION ${CMAKE_PACKAGE_DIR})

# Install larvc shared library
install(TARGETS ${name}
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
    INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
    )