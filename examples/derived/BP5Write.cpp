#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <chrono>

#include <adios2.h>
#include <mpi.h>

void write_derived1D(size_t Nx, size_t steps,
                     std::string derived_fct, int rank, int size,
                     std::string storeType)
{
    adios2::DerivedVarType deriveMode = adios2::DerivedVarType::StoreData;
    if (storeType == "MD")
        deriveMode = adios2::DerivedVarType::MetadataOnly;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 10.0);

    std::vector<float> simArray1(Nx);
    std::vector<float> simArray2(Nx);
    std::vector<float> simArray3(Nx);
    for (size_t i = 0; i < Nx; ++i)
    {
        simArray1[i] = distribution(generator);
        simArray2[i] = distribution(generator);
        simArray3[i] = distribution(generator);
    }

    adios2::ADIOS adios(MPI_COMM_WORLD);

    adios2::IO bpIO = adios.DeclareIO("BPExpression");
    bpIO.SetEngine("bp5");
    bpIO.DefineAttribute<int>("nsteps", steps);
    bpIO.DefineAttribute<int>("arraySize", Nx);
    bpIO.SetParameters("statslevel=1");
    bpIO.SetParameters("statsblocksize=5000");

    auto Ux =
        bpIO.DefineVariable<float>("uvel", {size * Nx}, {rank * Nx}, {Nx});
    auto Uy =
        bpIO.DefineVariable<float>("vvel", {size * Nx}, {rank * Nx}, {Nx});
    auto Uz =
        bpIO.DefineVariable<float>("wvel", {size * Nx}, {rank * Nx}, {Nx});

    if (derived_fct == "magnitude")
        auto magU = bpIO.DefineDerivedVariable(derived_fct,
                                           "x:uvel \n"
                                           "y:vvel \n"
                                           "z:wvel \n"
                                           "magnitude(x,y,z)",
                                           deriveMode);
    if (derived_fct == "add")
            auto addU = bpIO.DefineDerivedVariable(derived_fct,
                                           "x:uvel \n"
                                           "y:vvel \n"
                                           "z:wvel \n"
                                           "x+y+z",
                                           deriveMode);
    if (derived_fct == "pdf")
            auto PDFx = bpIO.DefineDerivedVariable(derived_fct,
                                           "x:uvel \n"
                                           "curl(100, x)",
                                           deriveMode);

    std::string filename = derived_fct+".bp";
    adios2::Engine bpFileWriter = bpIO.Open(filename, adios2::Mode::Write);

    for (int i = 0; i < steps; i++)
    {
        auto start_step = std::chrono::steady_clock::now();
        bpFileWriter.BeginStep();
        bpFileWriter.Put(Ux, simArray1.data());
        if (derived_fct != "pdf")
        {
            bpFileWriter.Put(Uy, simArray2.data());
            bpFileWriter.Put(Uz, simArray3.data());
        }
        bpFileWriter.EndStep();
        auto end_step = std::chrono::steady_clock::now();
        double timePerStep = (end_step - start_step).count() / 1000;

        double globalSum = 0;
        MPI_Reduce(&timePerStep, &globalSum, 1, MPI_DOUBLE, MPI_SUM, 0,
           MPI_COMM_WORLD);
        double globalMax = 0;
        MPI_Reduce(&timePerStep, &globalMax, 1, MPI_DOUBLE, MPI_MAX, 0,
           MPI_COMM_WORLD);
        double globalMin = 0;
        MPI_Reduce(&timePerStep, &globalMin, 1, MPI_DOUBLE, MPI_MIN, 0,
           MPI_COMM_WORLD);

        // Time in microseconds
        if (rank == 0)
            std::cout << "1D," << derived_fct << "," << storeType << ","
              << size << "," << Nx << "," << globalSum / size << "," << globalMin / size << ","
              << globalMax / size << std::endl;
    }

    bpFileWriter.Close();
    if (rank == 0)
    {
        std::cout << "All times are in microseconds" << std::endl;
        std::cout << "Wrote file " << filename << " to disk. \n";
    }
}

void write_derived3D(size_t Nx, size_t Ny, size_t Nz, size_t steps,
                     std::string derived_fct, int rank, int size,
                     std::string storeType)
{
    adios2::DerivedVarType deriveMode = adios2::DerivedVarType::StoreData;
    if (storeType == "MD")
        deriveMode = adios2::DerivedVarType::MetadataOnly;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 10.0);

    std::vector<float> simArray1(Nx * Ny * Nz);
    std::vector<float> simArray2(Nx * Ny * Nz);
    std::vector<float> simArray3(Nx * Ny * Nz);
    for (size_t i = 0; i < Nx * Ny * Nz; ++i)
    {
        simArray1[i] = distribution(generator);
        simArray2[i] = distribution(generator);
        simArray3[i] = distribution(generator);
    }

    adios2::ADIOS adios(MPI_COMM_WORLD);

    adios2::IO bpIO = adios.DeclareIO("BPExpression");
    bpIO.SetEngine("bp5");
    bpIO.DefineAttribute<int>("nsteps", steps);
    bpIO.DefineAttribute<int>("arraySize", Nx * Ny * Nz);
    bpIO.SetParameters("statslevel=1");
    bpIO.SetParameters("statsblocksize=5000");

    auto Ux =
        bpIO.DefineVariable<float>("uvel", {Nz, Ny, size * Nx}, {0, 0, rank * Nx}, {Nz, Ny, Nx});
    auto Uy =
        bpIO.DefineVariable<float>("vvel", {Nz, Ny, size * Nx}, {0, 0, rank * Nx}, {Nz, Ny, Nx});
    auto Uz =
        bpIO.DefineVariable<float>("wvel", {Nz, Ny, size * Nx}, {0, 0, rank * Nx}, {Nz, Ny, Nx});

    if (derived_fct == "magnitude")
        auto magU = bpIO.DefineDerivedVariable("derive/magU",
                                           "x:uvel \n"
                                           "y:vvel \n"
                                           "z:wvel \n"
                                           "magnitude(x,y,z)",
                                           deriveMode);
    if (derived_fct == "add")
            auto addU = bpIO.DefineDerivedVariable("derive/addU",
                                           "x:uvel \n"
                                           "y:vvel \n"
                                           "z:wvel \n"
                                           "x+y+z",
                                           deriveMode);
    if (derived_fct == "pdf")
            auto PDFx = bpIO.DefineDerivedVariable("derive/pdf",
                                           "x:uvel \n"
                                           "curl(100, x)",
                                           deriveMode);

    std::string filename = derived_fct+".bp";
    adios2::Engine bpFileWriter = bpIO.Open(filename, adios2::Mode::Write);

    for (int i = 0; i < steps; i++)
    {
        auto start_step = std::chrono::steady_clock::now();
        bpFileWriter.BeginStep();
        bpFileWriter.Put(Ux, simArray1.data());
        if (derived_fct != "pdf")
        {
            bpFileWriter.Put(Uy, simArray2.data());
            bpFileWriter.Put(Uz, simArray3.data());
        }
        bpFileWriter.EndStep();
        auto end_step = std::chrono::steady_clock::now();
        double timePerStep = (end_step - start_step).count() / 1000;

        double globalSum = 0;
        MPI_Reduce(&timePerStep, &globalSum, 1, MPI_DOUBLE, MPI_SUM, 0,
           MPI_COMM_WORLD);
        double globalMax = 0;
        MPI_Reduce(&timePerStep, &globalMax, 1, MPI_DOUBLE, MPI_MAX, 0,
           MPI_COMM_WORLD);
        double globalMin = 0;
        MPI_Reduce(&timePerStep, &globalMin, 1, MPI_DOUBLE, MPI_MIN, 0,
           MPI_COMM_WORLD);

        // Time in microseconds
        if (rank == 0)
            std::cout << "3D," << derived_fct << "," << storeType << ","
              << size << "," << Nx << "," << globalSum / size << "," << globalMin / size << ","
              << globalMax / size << std::endl;
    }

    bpFileWriter.Close();
    if (rank == 0)
    {
        std::cout << "All times are in microseconds" << std::endl;
        std::cout << "Wrote file " << filename << " to disk. \n";
    }
}

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cout << "Usage: " << argv[0] << " 1D/3D array_size steps derived_function DATA/MD"
                  << std::endl;
        return -1;
    }
    const std::string runType(argv[1]);
    const size_t Nx = atoi(argv[2]), Ny = Nx, Nz = Nx;
    const size_t steps = atoi(argv[3]);
    const std::string derivedFct(argv[4]);
    const std::string storeType(argv[5]);

    int rank, size;
    int provided;

    // MPI_THREAD_MULTIPLE is only required if you enable the SST MPI_DP
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "DerivedTest " << runType << " steps " << steps << " [ ";
    if (runType == "1D")
        std::cout << Nx << "]" << std::endl;
    if (runType == "3D")
        std::cout << Nx << " " << Ny << " " << Nz << "]" << std::endl;
    std::cout << "Function " << derivedFct << " type " << storeType << std::endl;

    if (runType == "1D")
        write_derived1D(Nx, steps, derivedFct, rank, size, storeType);
    if (runType == "3D")
        write_derived3D(Nx, Ny, Nz, steps, derivedFct, rank, size, storeType);

    MPI_Finalize();

    return 0;
}
