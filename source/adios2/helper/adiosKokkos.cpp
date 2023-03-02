/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */

#ifndef ADIOS2_HELPER_ADIOSKokkos_CPP_
#define ADIOS2_HELPER_ADIOSKokkos_CPP_

#include "adiosKokkos.h"
#include <Kokkos_Core.hpp>

namespace adios2
{
namespace helper
{
void KokkosInit() { Kokkos::initialize(); }
}
}

#endif /* ADIOS2_HELPER_ADIOSKokkos_CPP_ */
