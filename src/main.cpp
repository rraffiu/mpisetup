#include "setupmpi.h"
#include <iostream>

int main(int argc, char** argv)
{
    int nkpoints, ik_global;
    int ik_local_start, ik_local_end, mpi_rank;
    bool initialized;
    int rows = 4;
    int cols = 6;
    int iorank = 2; 
    int n_dm = rows*cols;
    if (argc < 2)
    {
    	printf("Number of K-points to distribute not provided.\n");
	exit(EXIT_FAILURE);
    }
    nkpoints = atoi(argv[1]);
    double* smatrix = new double[n_dm]{0}; 
    double** dmatrix = new double*[nkpoints];
    for (int i = 0; i < nkpoints; i++)
    {
        dmatrix[i] = new double[n_dm];
    }

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
        for (int id = 0; id < n_dm; id++)
        {
            dmatrix[ik_global][id] = mpi_rank; 
        }
           
        printf("Rank %d has kpoint %d with local index %d\n", mpi_rank, ik_global, ik);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // local addition
    for (int ik = 0; ik < ik_local_end - ik_local_start; ik++)
    {
        ik_global = ik + ik_local_start;
        for (int ib = 0; ib < n_dm; ib++)
        {
            smatrix[ib] += dmatrix[ik_global][ib];
        }
        
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (mpi_rank == iorank)
    {
        std::cout << "Printing local sum" <<std::endl;
        for (int ir = 0; ir < rows; ir++)
        {
            for (int ic = 0; ic < cols; ic++)
            {
                std::cout << smatrix[ir*cols + ic] << " ";
            }
            std::cout <<std::endl;
        }   
         std::cout << "Done printing local sum" <<std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, &smatrix[0], n_dm, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (mpi_rank == iorank)
    {
        std::cout << "Printing global sum" <<std::endl;
        for (int ir = 0; ir < rows; ir++)
        {
            for (int ic = 0; ic < cols; ic++)
            {
                std::cout << smatrix[ir*cols + ic] << " ";
            }
            std::cout <<std::endl;
        }   
         std::cout << "Done printing global sum" <<std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int ik = 0; ik < nkpoints; ik++)
    {
        MPI_Allreduce(MPI_IN_PLACE, &dmatrix[ik][0], n_dm, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    }
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
                    std::cout << dmatrix[ik][ir*cols + ic] << " ";
                }
                std::cout << std::endl;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    delete [] smatrix;
    for (int i = 0; i < nkpoints; i++)
    {
        delete[] dmatrix[i];   
    }
    delete[] dmatrix;
    return MPI_Finalize();
}
