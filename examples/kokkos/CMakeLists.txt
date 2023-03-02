find_package(Kokkos REQUIRED)

add_executable(KokkosHello kokkosHelloWorld.cpp)
target_link_libraries(KokkosHello PUBLIC adios2::cxx11 Kokkos::kokkos)
