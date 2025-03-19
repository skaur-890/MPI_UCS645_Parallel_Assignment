#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N (1 << 16)  // Vector size (2^16)
#define A 2.5        // Scalar value

void daxpy_serial(double *x, double *y, double a, int n){
    for(int i=0; i<n; i++){
        x[i]=a*x[i]+y[i];
    }
}

int main(int argc, char* argv[]){
    int rank, size;
    double *X, *Y, *local_X, *local_Y;
    int chunk_size;
    double start, end, serial_time, parallel_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    chunk_size=N/size;
    local_X=(double*)malloc(chunk_size*sizeof(double));
    local_Y=(double*)malloc(chunk_size*sizeof(double));

    if(rank==0){
        X=(double*)malloc(N*sizeof(double));
        Y=(double*)malloc(N*sizeof(double));

        for(int i=0; i<N; i++){
            X[i]=1.0;
            Y[i]=2.0;
        }

        start=MPI_Wtime();
        daxpy_serial(X, Y, A, N);
        end=MPI_Wtime();
        serial_time=end-start;
        printf("serial execution time: %f seconds\n", serial_time);
        
        for(int i=0; i<N; i++){
            X[i]=1.0;
            Y[i]=2.0;
        }
    }

    MPI_Scatter(
        X,
        chunk_size,
        MPI_DOUBLE,
        local_X,
        chunk_size,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    MPI_Scatter(
        Y,
        chunk_size,
        MPI_DOUBLE,
        local_Y,
        chunk_size,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    start=MPI_Wtime();
    for(int i=0; i<chunk_size; i++){
        local_X[i]=A*local_X[i]+local_Y[i];
    }
    end=MPI_Wtime();
    parallel_time=end-start;

    MPI_Gather(
        local_X,
        chunk_size,
        MPI_DOUBLE,
        X,
        chunk_size, 
        MPI_DOUBLE, 
        0,
        MPI_COMM_WORLD
    );

    if(rank==0){
        printf("Parallel execution time: %f seconds\n", parallel_time);
        printf("Speedup: %f\n", serial_time/parallel_time);
        free(X);
        free(Y);
    }

    free(local_X);
    free(local_Y);
    MPI_Finalize();
    return 0;
}