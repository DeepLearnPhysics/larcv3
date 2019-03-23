set(name dataformat)

project(${name})



# include_directories(include)

file(GLOB SOURCES *.cxx)

add_library(${name} OBJECT ${SOURCES})
target_link_libraries(dataformat PUBLIC ${HDF5_CXX_LIBRARIES}) 
target_link_libraries(dataformat PUBLIC base) 


# find_package(Python ${PYTHON_VERSION} REQUIRED COMPONENTS Development)
# find_package(PythonInterp ${PYTHON_BINDING_PYTHON_VERSION})

# message(${PYTHON_VERSION_MAJOR})

find_package(SWIG REQUIRED COMPONENTS C CXX)
include(${SWIG_USE_FILE})


file(GLOB SWIGSOURCES "*.i" )
set_source_files_properties(${SWIGSOURCES} PROPERTIES CPLUSPLUS ON)


swig_add_library(${name} 
    TYPE MODULE
    LANGUAGE python 
    SOURCES ${SWIGSOURCES}
    )

# swig_add_library(larcv
#     TYPE SHARED)

swig_link_libraries(${name} ${PYTHON_LIBRARIES} ${name} base)
# swig_add_library(larcv)

# install( TARGETS ${SWIG_MODULE_${name}_REAL_NAME}
#     LIBRARY DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/larcv )

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.py 
#     DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/larcv)

install( TARGETS ${SWIG_MODULE_${name}_REAL_NAME}
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR} )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.py 
    DESTINATION ${CMAKE_PACKAGE_DIR} )
