#include "setupmpi.h"

#include <multi/array.hpp>

#include <iostream>

int main(int argc, char** argv)
{
    int nkpoints, ik_global;
    int ik_local_start, ik_local_end, mpi_rank;
    bool initialized;
    int const rows = 4;
    int const cols = 6;
    int iorank = 2;

    if (argc < 2)
    {
    	printf("Number of K-points to distribute not provided.\n");
	exit(EXIT_FAILURE);
    }
    nkpoints = atoi(argv[1]);

    namespace multi = boost::multi;

    multi::array<double, 2> smatrix({rows, cols}, 0.0);

    multi::array<double, 3> dmatrix({nkpoints, rows, cols});

    setupmpi mpikpoint;
    mpikpoint.mpi_iniate();
    initialized = mpikpoint.distribute_kpoints(nkpoints);
    ik_local_start = mpikpoint.kstart;
    ik_local_end   = mpikpoint.kend;
    mpi_rank       = mpikpoint.mpi_rank;
    
    // local initialization
    
    for (int ik=0; ik < ik_local_end -ik_local_start; ik++)
    {
        ik_global = ik + ik_local_start;
        for(int ir = 0; ir != rows; ++ir) {
            for (int ic = 0; ic != cols; ++ic) {
                dmatrix[ik_global][ir][ic] = mpi_rank;
            }
        }
        printf("Rank %d has kpoint %d with local index %d\n", mpi_rank, ik_global, ik);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // local addition
    for (int ik = 0; ik < ik_local_end - ik_local_start; ik++)
    {
        ik_global = ik + ik_local_start;
        for(int n = 0; n != smatrix.elements().size(); ++n) {
            smatrix.elements()[n] += dmatrix[ik_global].elements()[n];
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_rank == iorank)
    {
        std::cout << "Printing local sum" <<std::endl;
        for (int ir = 0; ir < rows; ir++) {
            for (int ic = 0; ic < cols; ic++) {
                std::cout << smatrix[ir][ic] << " ";
            }
            std::cout <<std::endl;
        }   
         std::cout << "Done printing local sum" <<std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, smatrix.data_elements(), smatrix.num_elements(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (mpi_rank == iorank)
    {
        std::cout << "Printing global sum" <<std::endl;
        for (int ir = 0; ir < rows; ir++)
        {
            for (int ic = 0; ic < cols; ic++)
            {
                std::cout << smatrix[ir][ic] << " ";
            }
            std::cout <<std::endl;
        }   
         std::cout << "Done printing global sum" <<std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, dmatrix.data_elements(), dmatrix.num_elements(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_rank == iorank)
    {
        for (int ik = 0; ik < nkpoints; ik++)
        {
            ik_global = ik + ik_local_start;
            printf("\nRank %d has matrices: \n", mpi_rank);
            for (int ir = 0; ir < rows; ir++)
            {
                for (int ic=0; ic < cols; ic++)
                {
                    std::cout << dmatrix[ik][ir][ic] << " ";
                }
                std::cout << std::endl;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    return MPI_Finalize();
}
