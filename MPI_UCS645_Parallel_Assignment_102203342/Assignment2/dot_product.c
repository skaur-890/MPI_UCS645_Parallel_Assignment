#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 8 //size of vector

int main(int argc, char *argv[]){
    int rank, num_procs;
    int local_N;
    int A[N]={1,2,3,4,5,6,7,8};
    int B[N]={8,7,6,5,4,3,2,1};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    local_N=N/num_procs; //workload divide

    int local_A[local_N], local_B[local_N], local_dot_product=0, global_dot_product=0;


    //scattering data
    MPI_Scatter(
        A, 
        local_N, 
        MPI_INT, 
        local_A, 
        local_N, 
        MPI_INT, 
        0, 
        MPI_COMM_WORLD
    );
    MPI_Scatter(
        B, 
        local_N, 
        MPI_INT, 
        local_B, 
        local_N, 
        MPI_INT, 
        0, 
        MPI_COMM_WORLD
    );

    //compute dot product
    for(int i=0; i<local_N; i++){
        local_dot_product+=local_A[i]*local_B[i];
    }

    //reduce
    MPI_Reduce(
        &local_dot_product, 
        &global_dot_product, 
        1, 
        MPI_INT, 
        MPI_SUM, 
        0, 
        MPI_COMM_WORLD
    );

    if (rank == 0) {
        printf("Dot Product: %d\n", global_dot_product);
    }

    MPI_Finalize();
    return 0;
}