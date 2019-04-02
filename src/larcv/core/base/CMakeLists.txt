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
if (${CMAKE_MINOR_VERSION} GREATER 7)
  swig_add_library(${name} 
      TYPE MODULE
      LANGUAGE python 
      SOURCES ${SWIGSOURCES}
      )
else ()
  swig_add_module(
      ${name}
      python 
      ${SWIGSOURCES})
endif()




# Link to python and core shared library
swig_link_libraries(${name} ${PYTHON_LIBRARIES} ${name})
# swig_add_library(larcv)


# I took this from a github:
# https://github.com/pemsley/cmake-python-swig-rpath-example/blob/master/src/CMakeLists.txt
# I DON'T KNOW WHY IT WORKS BUT IT WORKS ON MAC
if (APPLE) 

   # interesting reading: https://blog.kitware.com/upcoming-in-cmake-2-8-12-osx-rpath-support/

   # INSTALL_NAME_DIR overrides MACOSX_RPATH.  Try not to do that.

   # this sets the install_name of lib${name}.dylib as @rpath/libtriv_stats.dylib
   SET_TARGET_PROPERTIES(${name} PROPERTIES MACOSX_RPATH TRUE)
   SET_TARGET_PROPERTIES(${name} PROPERTIES CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
   
   # Use rpath for _simple_module.so
   SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES MACOSX_RPATH TRUE)

   # append directories in the linker search path and outside the project to the INSTALL_RPATH
   SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

   # this is where lib${name}.dylib is installed relative to where ${SWIG_MODULE_${name}_REAL_NAME}.so is installed
   SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/lib/")

else()

   SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
   SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

endif()



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