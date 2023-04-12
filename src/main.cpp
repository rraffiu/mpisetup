#include "setupmpi.h"

int main(int argc, char** argv)
{
    int nkpoints;
    int ik_local_start, ik_local_end, mpi_rank;
    bool initialized;
    
    if (argc < 2)
    {
    	printf("Number of K-points to distribute not provided.\n");
	exit(EXIT_FAILURE);
    }
    nkpoints = atoi(argv[1]);
    setupmpi mpikpoint;
    mpikpoint.mpi_iniate();
    initialized = mpikpoint.distribute_kpoints(nkpoints);
    ik_local_start = mpikpoint.kstart;
    ik_local_end   = mpikpoint.kend;
    mpi_rank       = mpikpoint.mpi_rank;

    for (int ik=ik_local_start; ik < ik_local_end; ik++)
    {
        printf("Rank %d has kpoint %d\n", mpi_rank, ik);
    }
return MPI_Finalize();
}
