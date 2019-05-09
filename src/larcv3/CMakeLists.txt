add_subdirectory(core)
add_subdirectory(app)
# add_subdirectory(python)

# This is admittedly pretty clunky, but it gets the job done to make one object file:
add_library(larcv3 SHARED 
    $<TARGET_OBJECTS:base> 
    $<TARGET_OBJECTS:dataformat> 
    $<TARGET_OBJECTS:processor> 
    $<TARGET_OBJECTS:pyutil> 
    $<TARGET_OBJECTS:cpputil> 
    $<TARGET_OBJECTS:filter> 
    $<TARGET_OBJECTS:threadio> 
    )

# Link against hdf5:
target_link_libraries(larcv3 ${HDF5_CXX_LIBRARIES})

# Link against python:
target_link_libraries(larcv3 ${PYTHON_LIBRARIES})


    

# Install larvc shared library
install(TARGETS larcv3
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
    INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
    )
