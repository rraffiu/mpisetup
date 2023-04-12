#include "setupmpi.h"

setupmpi mpikpair;

bool setupmpi::distribute_kpoints(size_t nkpoints)
{
    MPI_Barrier(MPI_COMM_WORLD);
    for (int iproc = 0; iproc < mpi_nprocs; iproc++)
    {
        endArr.push_back((nkpoints * (iproc +1))/mpi_nprocs);
    }
    kstart = start(mpi_rank);
    kend   = end(mpi_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    return true;
}
