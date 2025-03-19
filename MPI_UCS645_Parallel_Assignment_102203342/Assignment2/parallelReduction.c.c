#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    int rank, num_procs;
    int global_sum, local_value;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    local_value=rank+1;

    MPI_Reduce(
        &local_value,
        &global_sum,
        1,
        MPI_INT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    if(rank==0){
        printf("total sum using MPI_Reduce: %d\n", global_sum);
    }

    MPI_Finalize();
    return 0;
}