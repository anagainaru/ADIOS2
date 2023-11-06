#include <ios>      //std::ios_base::failure
#include <iostream> //std::cout
#include <mpi.h>
#include <stdexcept> //std::invalid_argument std::exception
#include <vector>
#include <chrono>

#include <adios2.h>

void read_derived3D(size_t Nx, size_t Ny, size_t Nz, std::string derived_fct,
                    int rank, int size, std::string storeType)
{
}

void read_derived1D(size_t Nx, std::string derived_fct,
                    int rank, int size, std::string storeType)
{
}

void read_derived3D1(size_t Nx, size_t Ny, size_t Nz, std::string derived_fct,
                    int rank, int size, std::string storeType)
{
    std::string filename = derived_fct+".bp";

    adios2::ADIOS adios(MPI_COMM_WORLD);
    adios2::IO bpIO = adios.DeclareIO("ReadBP");
    adios2::Engine bpReader = bpIO.Open(filename, adios2::Mode::Read);

        bpReader.BeginStep();
        const std::map<std::string, adios2::Params> variables = bpIO.AvailableVariables();
        if (storeType == "DATA")
        {

        }
        adios2::Variable<float> bpFloats = bpIO.InquireVariable<float>(derived_fct);
        if (bpFloats)
        {
            std::vector<float> myFloats;

            bpFloats.SetSelection({{0, 0, rank * Nx}, {Nz, Ny, Nx}});
            bpReader.Get<float>(bpFloats, myFloats);
            bpReader.EndStep();
        }

        bpReader.Close();
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
