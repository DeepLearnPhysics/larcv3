set(name sbnd_imagemod)


# Get all the source files:
file(GLOB SOURCES *.cxx)
file(GLOB HEADERS *.h)

# Add a shared library
add_library(${name} OBJECT ${SOURCES})





install (FILES ${HEADERS}
    DESTINATION ${CMAKE_PACKAGE_DIR}/include/larcv3/app/${name})
