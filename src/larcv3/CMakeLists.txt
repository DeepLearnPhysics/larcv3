add_subdirectory(core)
add_subdirectory(app)


# This is admittedly pretty clunky, but it gets the job done to make one object file:

add_library(larcv3 SHARED
    $<TARGET_OBJECTS:base>
    $<TARGET_OBJECTS:dataformat>
    $<TARGET_OBJECTS:processor>
    $<TARGET_OBJECTS:filter>
    $<TARGET_OBJECTS:queueio>
    $<TARGET_OBJECTS:imagemod>
    $<TARGET_OBJECTS:sbnd_imagemod>
    $<TARGET_OBJECTS:augment>
    )



if (OpenMP_CXX_FOUND)
    message("Linking against openmp")
    message("OpenMP_CXX_LIBRARY: " ${OpenMP_CXX_LIBRARIES})
    message("OpenMP_C_LIBRARY: " ${OpenMP_C_LIBRARIES})
    target_link_libraries(larcv3 OpenMP::OpenMP_C)
endif()

if (MPI_FOUND)
    message("Linking against MPI")
    message("OpenMP_CXX_LIBRARY: " ${MPI_CXX_LIBRARIES})
    message("OpenMP_C_LIBRARY: " ${MPI_C_LIBRARIES})
    target_link_libraries(larcv3 ${MPI_CXX_LIBRARIES} ${MPI_C_LIBRARIES})
endif()

set(HDF5_PREFER_PARALLEL TRUE)
message("Using hdf5 parallel: " ${HDF5_IS_PARALLEL})
target_link_libraries(larcv3 ${HDF5_LIBRARIES})

# Link against python:
target_link_libraries(larcv3 ${Python_LIBRARIES})


###############################

# Link against the json library:
target_link_libraries(larcv3 nlohmann_json::nlohmann_json)


if(CMAKE_PYTHON_BINDINGS)
    pybind11_add_module(pylarcv SHARED larcv.cxx)

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

###############################

if (MPI)
    target_link_libraries(larcv3 ${MPI_LIBRARIES})
endif()



# Install larvc shared library
install(TARGETS larcv3
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
    INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
    )


# Define the location of the header files, for doc generation
set(LARCV3_PUBLIC_HEADER_DIR ${CMAKE_PACKAGE_DIR}/include)
message("Header dir ${CMAKE_PACKAGE_DIR}/include")
# set(LARCV3_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR})


if(CMAKE_PYTHON_BINDINGS)
    # Install larvc wrapper library
    install(TARGETS pylarcv
        LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}
        ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}
        # RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
        # INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
        )
endif(CMAKE_PYTHON_BINDINGS)
