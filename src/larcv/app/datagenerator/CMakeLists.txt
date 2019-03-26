project(datagenerator)

# include_directories(include)

file(GLOB SOURCES *.cxx)

add_library(datagenerator OBJECT ${SOURCES})
target_link_libraries(datagenerator PUBLIC base dataformat processor) 


# install(TARGETS datagenerator
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION include
#     )

# find_package(PythonLibs REQUIRED)
# include_directories(${PYTHON_INCLUDE_PATH})


# find_package(SWIG REQUIRED COMPONENTS C CXX)
# include(${SWIG_USE_FILE})

# set(CMAKE_SWIG_FLAGS "")

# set_source_files_properties(datagenerator.i PROPERTIES CPLUSPLUS ON)
# # set_source_files_properties(datagenerator.i PROPERTIES SWIG_FLAGS "-includeall")


# swig_add_library(datagenerator 
#     TYPE SHARED
#     LANGUAGE python 
#     SOURCES datagenerator.i)


# swig_link_libraries(datagenerator ${PYTHON_LIBRARIES} datagenerator)

# # install(TARGETS datagenerator
# #     LIBRARY DESTINATION lib
# #     ARCHIVE DESTINATION lib
# #     RUNTIME DESTINATION bin
# #     INCLUDES DESTINATION include)

# set(SETUP_PY_OUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py)


# # Install target to call setup.py
# add_custom_target(install-python
#         DEPENDS _swig_example
#         COMMAND python ${SETUP_PY_OUT} install)