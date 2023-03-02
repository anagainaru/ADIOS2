#include <iostream>
#include <Kokkos_Core.hpp>


int main(int argc, char **argv)
{
    Kokkos::initialize(argc, argv);
    {
		std::cout << "Hello World" << std::endl;
    }
    Kokkos::finalize();
    return 1;
}
