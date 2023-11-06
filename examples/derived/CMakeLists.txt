add_executable(bpExpression_write BP5Write.cpp)
target_link_libraries(bpExpression_write MPI::MPI_C adios2::cxx11 adios2::cxx11_mpi)

add_executable(bpExpression_read BP5Read.cpp)
target_link_libraries(bpExpression_read MPI::MPI_C adios2::cxx11 adios2::cxx11_mpi)
