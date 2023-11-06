#include <ios>      //std::ios_base::failure
#include <iostream> //std::cout
#include <mpi.h>
#include <stdexcept> //std::invalid_argument std::exception
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>

#include <adios2.h>



void compute_magnitude(std::vector<float> U, std::vector<float> V, std::vector<float> W)
{
    std::vector<float> mag(U.size());
    for (size_t i = 0; i < U.size(); i++)
    {
        mag[i] = std::sqrt(U[i] * U[i] + V[i] * V[i] + W[i] * W[i]);
    }
}

void read_derived1D(size_t Nx, std::string derived_fct,
                    int rank, int size, std::string storeType)
{
}

void read_derived3D(size_t Nx, size_t Ny, size_t Nz, std::string derived_fct,
                    int rank, int size, std::string storeType)
{
    std::string filename = derived_fct+".bp";

    adios2::ADIOS adios(MPI_COMM_WORLD);
    adios2::IO bpIO = adios.DeclareIO("ReadBP");
    adios2::Engine bpReader = bpIO.Open(filename, adios2::Mode::Read);

    for (size_t step=0; step<3; step++)
    {
        auto start_step = std::chrono::steady_clock::now();
        bpReader.BeginStep();
        const std::map<std::string, adios2::Params> variables = bpIO.AvailableVariables();
        if (storeType == "DATA")
        {
            adios2::Variable<float> bpFloats = bpIO.InquireVariable<float>(derived_fct);
            if (bpFloats)
            {
            std::vector<float> myFloats;
                bpFloats.SetSelection({{0, 0, rank * Nx}, {Nz, Ny, Nx}});
                bpReader.Get<float>(bpFloats, myFloats);
            }
            bpReader.EndStep();
        }
        if (storeType == "MD")
        {
            // read all the variables that make the derived variable and compute the vallues
            adios2::Variable<float> bpU = bpIO.InquireVariable<float>("uvel");
            adios2::Variable<float> bpV = bpIO.InquireVariable<float>("vvel");
            adios2::Variable<float> bpW = bpIO.InquireVariable<float>("wvel");

            std::vector<float> U;
            std::vector<float> V;
            std::vector<float> W;
            bpU.SetSelection({{0, 0, rank * Nx}, {Nz, Ny, Nx}});
            bpV.SetSelection({{0, 0, rank * Nx}, {Nz, Ny, Nx}});
            bpW.SetSelection({{0, 0, rank * Nx}, {Nz, Ny, Nx}});

            bpReader.Get<float>(bpU, U);
            bpReader.Get<float>(bpV, V);
            bpReader.Get<float>(bpW, W);
            bpReader.EndStep();
            compute_magnitude(U,V,W);
        }

        auto end_step = std::chrono::steady_clock::now();
        double timePerStep = (end_step - start_step).count() / 1000;            
        bpReader.Close();

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
            std::cout << "Read3D," << derived_fct << "," << storeType << ","
              << size << "," << Nx << "," << globalSum / size << "," << globalMin / size << ","
              << globalMax / size << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cout << "Usage: " << argv[0] << " 1D/3D array_size derived_function DATA/MD"
                  << std::endl;
        return -1;
    }
    int provided;
    const std::string runType(argv[1]);
    const size_t Nx = atoi(argv[2]), Ny = Nx, Nz = Nx;
    const std::string derivedFct(argv[3]);
    const std::string storeType(argv[4]);

    // MPI_THREAD_MULTIPLE is only required if you enable the SST MPI_DP
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "DerivedTest " << runType << " [ ";
    if (runType == "1D")
        std::cout << Nx << "]" << std::endl;
    if (runType == "3D")
        std::cout << Nx << " " << Ny << " " << Nz << "]" << std::endl;
    std::cout << "Function " << derivedFct << " type " << storeType << std::endl;

    std::string filename = derivedFct+".bp";
    try
    {
        if (runType == "1D")
            read_derived1D(Nx, derivedFct, rank, size, storeType);
        if (runType == "3D")
            read_derived3D(Nx, Ny, Nz, derivedFct, rank, size, storeType);
    }
    catch (std::invalid_argument &e)
    {
        if (rank == 0)
        {
            std::cerr << "Invalid argument exception, STOPPING PROGRAM from rank " << rank << "\n";
            std::cerr << e.what() << "\n";
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    catch (std::ios_base::failure &e)
    {
        if (rank == 0)
        {
            std::cerr << "IO System base failure exception, STOPPING PROGRAM "
                         "from rank "
                      << rank << "\n";
            std::cerr << e.what() << "\n";
            std::cerr << "The file " << filename << " does not exist."
                      << " Presumably this is because hello_bpWriter has not "
                         "been run."
                      << " Run ./hello_bpWriter before running this program.\n";
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    catch (std::exception &e)
    {
        if (rank == 0)
        {
            std::cerr << "Exception, STOPPING PROGRAM from rank " << rank << "\n";
            std::cerr << e.what() << "\n";
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Finalize();

    return 0;
}
