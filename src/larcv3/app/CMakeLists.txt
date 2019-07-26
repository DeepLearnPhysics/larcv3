add_subdirectory(filter)
add_subdirectory(threadio)
add_subdirectory(imagemod)
add_subdirectory(sbnd_imagemod)

if (MPI)
    add_subdirectory(mpi_io)
endif()