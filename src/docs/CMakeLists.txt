# This is based off of the blog post from here:
# https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

find_package(Doxygen REQUIRED)

if (DOXYGEN_FOUND)

    # Find all the public headers
    get_target_property(LARCV3_PUBLIC_HEADER_DIR larcv3 INTERFACE_INCLUDE_DIRECTORIES)
    file(GLOB_RECURSE LARCV3_PUBLIC_HEADERS ${LARCV3_PUBLIC_HEADER_DIR}/*.h)


    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)

    # Set the output location
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${LARCV3_PUBLIC_HEADERS}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )

    add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

    find_package(Sphinx REQUIRED)
    if (SPHINX_FOUND)
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
        set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
        set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

        # Only regenerate Sphinx when:
        # - Doxygen has rerun
        # - Our doc files have been updated
        # - The Sphinx config has been updated
        add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                           COMMAND 
                             ${SPHINX_EXECUTABLE} -b html
                             # Tell Breathe where to find the Doxygen output
                             -Dbreathe_projects.larcv3=${DOXYGEN_OUTPUT_DIR}/xml
                           ${SPHINX_SOURCE} ${SPHINX_BUILD}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           DEPENDS
                           # Other docs files you want to track should go here (or in some variable)
                           ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                           ${DOXYGEN_INDEX_FILE}
                           MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                           COMMENT "Generating documentation with Sphinx")

        # Nice named target so we can run the job easily
        add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

        # Add an install target to install the docs
        include(GNUInstallDirs)
        install(DIRECTORY ${SPHINX_BUILD}
                DESTINATION ${CMAKE_INSTALL_DOCDIR})

    endif (SPHINX_FOUND)


else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

