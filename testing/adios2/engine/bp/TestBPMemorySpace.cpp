/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */

#include <cstdint>
#include <cstring>

#include <iostream>
#include <stdexcept>

#include <adios2.h>

#include <gtest/gtest.h>

std::string engineName;

class BPMemorySpace: public ::testing::Test
{
public:
    BPMemorySpace() = default;
};

TEST_F(BPMemorySpace, BPDefaultMemorySpace)
{
    adios2::ADIOS adios;
    adios2::IO outIO = adios.DeclareIO("Output");

    if (!engineName.empty())
    {
        outIO.SetEngine(engineName);
    }

    auto var = outIO.DefineVariable<double>("v", {10, 1}, {0, 0}, {10, 1});
    EXPECT_EQ(var.GetMemorySpace(), adios2::MemorySpace::Detect);
}

TEST_F(BPMemorySpace, BPSetMemorySpace)
{
    adios2::ADIOS adios;
    adios2::IO outIO = adios.DeclareIO("Output");

    if (!engineName.empty())
    {
        outIO.SetEngine(engineName);
    }

    auto var = outIO.DefineVariable<double>("v", {10, 1}, {0, 0}, {10, 1});
    var.SetMemorySpace(adios2::MemorySpace::Host);
    EXPECT_EQ(var.GetMemorySpace(), adios2::MemorySpace::Host);
    // check that it throws exception if trying to set back to Detect
}

int main(int argc, char **argv)
{
    int result;
    ::testing::InitGoogleTest(&argc, argv);

    if (argc > 1)
    {
        engineName = std::string(argv[1]);
    }

    result = RUN_ALL_TESTS();
    return result;
}
