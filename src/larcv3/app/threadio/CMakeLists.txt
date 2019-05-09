set(name threadio)

# Get all the source files:
file(GLOB SOURCES *.cxx)
file(GLOB HEADERS *.h)

# Add a shared library
add_library(${name} OBJECT ${SOURCES})


generate_python_bindings(${name})

install (FILES ${HEADERS}
    DESTINATION ${CMAKE_PACKAGE_DIR}/include/app/${name})
