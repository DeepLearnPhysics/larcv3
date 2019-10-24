add_subdirectory(core)
add_subdirectory(app)

# This is admittedly pretty clunky, but it gets the job done to make one object file:

add_library(larcv3 SHARED 
    $<TARGET_OBJECTS:base> 
    $<TARGET_OBJECTS:dataformat> 
    $<TARGET_OBJECTS:processor> 
    $<TARGET_OBJECTS:pyutil> 
    $<TARGET_OBJECTS:cpputil> 
    $<TARGET_OBJECTS:filter> 
    $<TARGET_OBJECTS:threadio> 
    $<TARGET_OBJECTS:imagemod> 
    $<TARGET_OBJECTS:sbnd_imagemod> 
    )

if (OpenMP_CXX_FOUND)
    message("Linking against openmp")
    message("OpenMP_CXX_LIBRARY: " ${OpenMP_CXX_LIBRARIES})
    message("OpenMP_C_LIBRARY: " ${OpenMP_C_LIBRARIES})
    target_link_libraries(larcv3 ${OpenMP_CXX_LIBRARIES} ${OpenMP_C_LIBRARIES})
endif()

# Link against hdf5:
target_link_libraries(larcv3 ${HDF5_C_LIBRARIES})

# Link against python:
target_link_libraries(larcv3 ${PYTHON_LIBRARIES})
 

if (MPI)
    target_link_libraries(larcv3 ${MPI_LIBRARIES})
endif()

if(${CMAKE_NO_SWIG})
    message("Swig bingings turned OFF")

else ()
    # Make sure swig is available:
    find_package(SWIG REQUIRED COMPONENTS C CXX)
    include(${SWIG_USE_FILE})

    include_directories(${PYTHON_NUMPY_INCLUDE_DIR})

    # Find the swig source file:
    file(GLOB SWIGSOURCES "larcv.i" )
    # Designate that this is a C++ file
    set_source_files_properties(${SWIGSOURCES} PROPERTIES CPLUSPLUS ON)

    # Add as a module ('.so' extension on mac)
    if (${CMAKE_MINOR_VERSION} GREATER 7)
      swig_add_library(larcv 
          TYPE MODULE
          LANGUAGE python 
          SOURCES ${SWIGSOURCES}
      )
    else ()
      swig_add_module(
          larcv
          python 
          ${SWIGSOURCES}
      )
    endif()


    # Link to python and core shared library
    swig_link_libraries(larcv ${PYTHON_LIBRARIES} larcv3)

    # I took this from a github:
    # https://github.com/pemsley/cmake-python-swig-rpath-example/blob/master/src/CMakeLists.txt
    # I DON'T KNOW WHY IT WORKS BUT IT WORKS ON MAC
    if (APPLE) 

        # interesting reading: https://blog.kitware.com/upcoming-in-cmake-2-8-12-osx-rpath-support/
        # INSTALL_NAME_DIR overrides MACOSX_RPATH.  Try not to do that.
        # this sets the install_name of lib${name}.dylib as @rpath/libtriv_stats.dylib
        set_target_properties(${SWIG_MODULE_larcv_REAL_NAME} PROPERTIES MACOSX_RPATH TRUE)
        set_target_properties(${SWIG_MODULE_larcv_REAL_NAME} PROPERTIES CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

        # Use rpath for _simple_module.so
        set_target_properties(${SWIG_MODULE_larcv_REAL_NAME} PROPERTIES MACOSX_RPATH TRUE)

        # append directories in the linker search path and outside the project to the INSTALL_RPATH
        set_target_properties(${SWIG_MODULE_larcv_REAL_NAME} PROPERTIES CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

        # this is where liblarcv.dylib is installed relative to where ${SWIG_MODULE_larcv_REAL_NAME}.so is installed
        set_target_properties(${SWIG_MODULE_larcv_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/lib/")

    else()
        set_target_properties(${SWIG_MODULE_larcv_REAL_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/lib/")

    endif()

    # Install swig module
    install( TARGETS ${SWIG_MODULE_larcv_REAL_NAME}
        LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR} )

    # Install swig python module
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/larcv.py 
        DESTINATION ${CMAKE_PACKAGE_DIR})

endif(${CMAKE_NO_SWIG})
    

# Install larvc shared library
install(TARGETS larcv3
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
    INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
    )
