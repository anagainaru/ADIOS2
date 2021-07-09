/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * TestBPAvailableVariablesAttributes.cpp
 *
 *  Created on: 7/9/21.
 *      Author: Dmitry Ganyushin ganyushindi@ornl.gov
 */

#include <adios2_c.h>

#if ADIOS2_USE_MPI
#include <mpi.h>
#endif

#include <gtest/gtest.h>

#include <numeric> //std::iota
#include <thread>
#include <algorithm>
#include "SmallTestData_c.h"

class BPAvailableVariablesAttributes : public ::testing::Test
{
public:
    BPAvailableVariablesAttributes() = default;
};

TEST_F(BPAvailableVariablesAttributes, AvailableVariablesAttributes)
{
    int rank = 0;
    int size = 1;
    size_t steps = 5;

#if ADIOS2_USE_MPI
    adios2_adios *adiosH = adios2_init(MPI_COMM_WORLD, adios2_debug_mode_on);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
#else
    adios2_adios *adiosH = adios2_init(adios2_debug_mode_on);
#endif

    // count dims are allocated in stack
    size_t shape[1];
    shape[0] = data_Nx * static_cast<size_t>(size);

    size_t start[1];
    start[0] = data_Nx * static_cast<size_t>(rank);

    size_t count[1];
    count[0] = data_Nx;

    adios2_step_status status;
    const std::vector<std::size_t> startNull = {0};
    const std::vector<std::size_t> countNull = {0};
    const std::vector<std::size_t> startValid = {start[0] + data_Nx / 2};
    const std::vector<std::size_t> countValid = {data_Nx / 2};
    void *nullPointer = nullptr;

    // write
    {
        // IO
        adios2_io *ioH = adios2_declare_io(adiosH, "CArrayTypes");
        // Set engine parameters
        adios2_set_engine(ioH, "BPFile");
        adios2_set_parameter(ioH, "ProfileUnits", "Microseconds");

        adios2_define_attribute(ioH, "strvalue", adios2_type_string,
                                "Testing zero size blocks with null pointer");

        // Define variables in ioH

        adios2_variable *varI8 =
            adios2_define_variable(ioH, "varI8", adios2_type_int8_t, 1, shape,
                                   start, count, adios2_constant_dims_false);
        adios2_variable *varI16 =
            adios2_define_variable(ioH, "varI16", adios2_type_int16_t, 1, shape,
                                   start, count, adios2_constant_dims_false);
        adios2_variable *varI32 =
            adios2_define_variable(ioH, "varI32", adios2_type_int32_t, 1, shape,
                                   start, count, adios2_constant_dims_false);
        adios2_variable *varI64 =
            adios2_define_variable(ioH, "varI64", adios2_type_int64_t, 1, shape,
                                   start, count, adios2_constant_dims_false);

        adios2_variable *varU8 =
            adios2_define_variable(ioH, "varU8", adios2_type_uint8_t, 1, shape,
                                   start, count, adios2_constant_dims_false);
        adios2_variable *varU16 = adios2_define_variable(
            ioH, "varU16", adios2_type_uint16_t, 1, shape, start, count,
            adios2_constant_dims_false);
        adios2_variable *varU32 = adios2_define_variable(
            ioH, "varU32", adios2_type_uint32_t, 1, shape, start, count,
            adios2_constant_dims_false);
        adios2_variable *varU64 = adios2_define_variable(
            ioH, "varU64", adios2_type_uint64_t, 1, shape, start, count,
            adios2_constant_dims_false);

        adios2_variable *varR32 =
            adios2_define_variable(ioH, "varR32", adios2_type_float, 1, shape,
                                   start, count, adios2_constant_dims_false);
        adios2_variable *varR64 =
            adios2_define_variable(ioH, "varR64", adios2_type_double, 1, shape,
                                   start, count, adios2_constant_dims_false);

        adios2_engine *engineH =
            adios2_open(ioH, "available.bp", adios2_mode_write);

        for (size_t i = 0; i < steps; ++i)
        {
            adios2_begin_step(engineH, adios2_step_mode_append, -1., &status);

            adios2_set_selection(varI8, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varI8, nullptr, adios2_mode_sync);
            adios2_set_selection(varI8, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varI8, &data_I8[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varI16, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varI16, nullptr, adios2_mode_sync);
            adios2_set_selection(varI16, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varI16, &data_I16[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varI32, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varI32, nullptr, adios2_mode_sync);
            adios2_set_selection(varI32, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varI32, &data_I32[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varI64, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varI64, nullptr, adios2_mode_sync);
            adios2_set_selection(varI64, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varI64, &data_I64[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varU8, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varU8, nullPointer, adios2_mode_deferred);
            adios2_set_selection(varU8, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varU8, &data_U8[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varU16, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varU16, nullPointer, adios2_mode_deferred);
            adios2_set_selection(varU16, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varU16, &data_U16[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varU32, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varU32, nullPointer, adios2_mode_deferred);
            adios2_set_selection(varU32, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varU32, &data_U32[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varU64, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varU64, nullPointer, adios2_mode_deferred);
            adios2_set_selection(varU64, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varU64, &data_U64[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varR32, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varR32, nullPointer, adios2_mode_deferred);
            adios2_set_selection(varR32, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varR32, &data_R32[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_set_selection(varR64, 1, startNull.data(), countNull.data());
            adios2_put(engineH, varR64, nullPointer, adios2_mode_deferred);
            adios2_set_selection(varR64, 1, startValid.data(),
                                 countValid.data());
            adios2_put(engineH, varR64, &data_R64[data_Nx / 2],
                       adios2_mode_deferred);

            adios2_end_step(engineH);
        }
        adios2_close(engineH);
    }
#if ADIOS2_USE_MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif
    {
        std::vector<int8_t> inI8(data_Nx / 2);
        std::vector<int16_t> inI16(data_Nx / 2);
        std::vector<int32_t> inI32(data_Nx / 2);
        std::vector<int64_t> inI64(data_Nx / 2);

        std::vector<uint8_t> inU8(data_Nx / 2);
        std::vector<uint16_t> inU16(data_Nx / 2);
        std::vector<uint32_t> inU32(data_Nx / 2);
        std::vector<uint64_t> inU64(data_Nx / 2);

        std::vector<float> inR32(data_Nx / 2);
        std::vector<double> inR64(data_Nx / 2);

        adios2_io *ioH = adios2_declare_io(adiosH, "Reader");
        adios2_engine *engineH =
            adios2_open(ioH, "available.bp", adios2_mode_read);

        size_t nsteps;
        adios2_steps(&nsteps, engineH);
        EXPECT_EQ(nsteps, steps);

        while (adios2_begin_step(engineH, adios2_step_mode_read, -1.,
                                 &status) == adios2_error_none)
        {
            if (status == adios2_step_status_end_of_stream)
            {
                break;
            }

            std::vector<std::string> correct_vars = {
                "varI16", "varI32", "varI64", "varI8",  "varR32",
                "varR64", "varU16", "varU32", "varU64", "varU8"};
            std::vector<std::string> correct_attrs = {"strvalue"};

            std::vector<std::string> vars;
            size_t var_size;
            char **var_names = adios2_available_variables(ioH, &var_size);
            for (size_t i = 0; i < var_size; i++)
            {
                vars.push_back(var_names[i]);
                free(var_names[i]);
            }
            free(var_names);
            std::sort(correct_vars.begin(), correct_vars.end());
            std::sort(vars.begin(), vars.end());
            EXPECT_EQ(correct_vars, vars);

            size_t attr_size;
            std::vector<std::string> attrs;
            char **attr_names = adios2_available_attributes(ioH, &attr_size);
            for (size_t i = 0; i < attr_size; i++)
            {
                attrs.push_back(attr_names[i]);
                free(attr_names[i]);
            }
            // remove memory
            free(attr_names);
            std::sort(attrs.begin(), attrs.end());
            std::sort(correct_attrs.begin(), correct_attrs.end());
            EXPECT_EQ(correct_attrs, attrs);

            adios2_variable *varI8 = adios2_inquire_variable(ioH, "varI8");
            adios2_set_selection(varI8, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varI8, inI8.data(), adios2_mode_deferred);

            adios2_variable *varI16 = adios2_inquire_variable(ioH, "varI16");
            adios2_set_selection(varI16, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varI16, inI16.data(), adios2_mode_deferred);

            adios2_variable *varI32 = adios2_inquire_variable(ioH, "varI32");
            adios2_set_selection(varI32, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varI32, inI32.data(), adios2_mode_deferred);

            adios2_variable *varI64 = adios2_inquire_variable(ioH, "varI64");
            adios2_set_selection(varI64, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varI64, inI64.data(), adios2_mode_deferred);

            adios2_variable *varU8 = adios2_inquire_variable(ioH, "varU8");
            adios2_set_selection(varU8, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varU8, inU8.data(), adios2_mode_deferred);

            adios2_variable *varU16 = adios2_inquire_variable(ioH, "varU16");
            adios2_set_selection(varU16, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varU16, inU16.data(), adios2_mode_deferred);

            adios2_variable *varU32 = adios2_inquire_variable(ioH, "varU32");
            adios2_set_selection(varU32, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varU32, inU32.data(), adios2_mode_deferred);

            adios2_variable *varU64 = adios2_inquire_variable(ioH, "varU64");
            adios2_set_selection(varU64, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varU64, inU64.data(), adios2_mode_deferred);

            adios2_variable *varR32 = adios2_inquire_variable(ioH, "varR32");
            adios2_set_selection(varR32, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varR32, inR32.data(), adios2_mode_deferred);

            adios2_variable *varR64 = adios2_inquire_variable(ioH, "varR64");
            adios2_set_selection(varR64, 1, startValid.data(),
                                 countValid.data());
            adios2_get(engineH, varR64, inR64.data(), adios2_mode_deferred);

            adios2_perform_gets(engineH);

            for (size_t i = 0; i < data_Nx / 2; ++i)
            {
                EXPECT_EQ(inI8[i], data_I8[data_Nx / 2 + i]);
                EXPECT_EQ(inI16[i], data_I16[data_Nx / 2 + i]);
                EXPECT_EQ(inI32[i], data_I32[data_Nx / 2 + i]);
                EXPECT_EQ(inI64[i], data_I64[data_Nx / 2 + i]);

                EXPECT_EQ(inU8[i], data_U8[data_Nx / 2 + i]);
                EXPECT_EQ(inU16[i], data_U16[data_Nx / 2 + i]);
                EXPECT_EQ(inU32[i], data_U32[data_Nx / 2 + i]);
                EXPECT_EQ(inU64[i], data_U64[data_Nx / 2 + i]);

                EXPECT_EQ(inR32[i], data_R32[data_Nx / 2 + i]);
                EXPECT_EQ(inR64[i], data_R64[data_Nx / 2 + i]);
            }
            adios2_end_step(engineH);
        }

        adios2_close(engineH);
    }
    // deallocate adiosH
    adios2_finalize(adiosH);
}

//******************************************************************************
// main
//******************************************************************************

int main(int argc, char **argv)
{
#if ADIOS2_USE_MPI
    MPI_Init(nullptr, nullptr);
#endif

    int result;
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

#if ADIOS2_USE_MPI
    MPI_Finalize();
#endif

    return result;
}