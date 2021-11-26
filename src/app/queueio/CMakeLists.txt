set(name queueio)

# Get all the source files:
file(GLOB SOURCES *.cxx)

# Add a shared library
add_library(${name} OBJECT ${SOURCES})



install (FILES ${HEADERS}
    DESTINATION ${CMAKE_PACKAGE_DIR}/include/app/${name})
