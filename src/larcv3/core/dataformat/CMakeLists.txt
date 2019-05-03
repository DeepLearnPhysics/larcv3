set(name dataformat)


project(${name})



# Get all the source files:
file(GLOB SOURCES *.cxx)

# Add a shared library
add_library(${name} SHARED ${SOURCES})

# Link this shared library against the base library
target_link_libraries(${name} ${HDF5_CXX_LIBRARIES} base)

generate_python_bindings(${name})

# Install larvc shared library
install(TARGETS ${name}
    LIBRARY DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_PACKAGE_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_PACKAGE_DIR}/bin
    INCLUDES DESTINATION ${CMAKE_PACKAGE_DIR}/include
    )