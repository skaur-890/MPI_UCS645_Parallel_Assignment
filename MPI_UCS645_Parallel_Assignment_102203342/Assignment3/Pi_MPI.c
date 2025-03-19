#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define NUM_STEPS 10000

int main(int argc, char* argv[]){
    int size, rank;
    double step, local_sum=0.0, x, partial_pi, pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long num_steps=NUM_STEPS;
    step=1.0/(double)num_steps;

    MPI_Bcast(
        &num_steps,
        1,
        MPI_LONG,
        0,
        MPI_COMM_WORLD
    );

    for(int i=rank; i<num_steps; i+=size){
        x=(i+0.5)*step;
        local_sum+=4.0/(1.0+x*x);
    }

    partial_pi=step*local_sum;

    MPI_Reduce(
        &partial_pi,
        &pi,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    if(rank==0){
        printf("Approximaiton of Pi: %f\n", pi);
    }

    MPI_Finalize();
    return 0;
}