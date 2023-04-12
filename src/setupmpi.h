#ifndef SETUPMPI_H
#define SETUPMPI_H

#include <mpi.h>
#include <vector>
#include <string>

class setupmpi
{
private:
public:
    bool ionode;
    int mpi_nprocs, mpi_rank;
    size_t kstart, kend;
    std::vector<size_t> endArr;
   
    setupmpi () : mpi_nprocs(0) {};

    bool mpi_initiated () {return mpi_nprocs > 0;}

    void mpi_iniate()
    {
        int flag = 0;
        MPI_Initialized(&flag);
        if (!flag) MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &mpi_nprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
        ionode = (mpi_rank == 0);
    };
    bool distribute_kpoints(size_t nkpoints);
    inline size_t start (int iproc) { return iproc ? endArr[iproc - 1] :0; }
    inline size_t end (int iproc) { return endArr [iproc];}
};


#endif
