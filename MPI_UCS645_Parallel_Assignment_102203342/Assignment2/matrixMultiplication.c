#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 70

void multiply_matrices(double A[N][N], double B[N][N], double C[N][N], int start, int end){
    for(int i=start; i<end; i++){
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char* argv[]){
    int rank, size;
    double A[N][N], B[N][N], C[N][N];
    double local_A[N][N], local_C[N][N];
    double start_time, run_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int rows_per_process=N/size;
    int start=rank*rows_per_process;
    int end=(rank == size - 1) ? N : start + rows_per_process;

    if(rank==0){
        for(int i=0; i<N; i++){
            for(int j=0; j<N; j++){
                A[i][j]=rand()%10;
                B[i][j]=rand()%10;
            }
        }
        start_time=MPI_Wtime();
    }

    MPI_Bcast(
        B, 
        N*N,
        MPI_DOUBLE, 
        0, 
        MPI_COMM_WORLD
    );

    MPI_Scatter(
        A, 
        rows_per_process*N, 
        MPI_DOUBLE, 
        local_A, 
        rows_per_process*N, 
        MPI_DOUBLE, 
        0, 
        MPI_COMM_WORLD
    );
    
    multiply_matrices(local_A, B, local_C, start, end);
    
    MPI_Gather(
        local_C, 
        rows_per_process*N, 
        MPI_DOUBLE,
        C, 
        rows_per_process*N, 
        MPI_DOUBLE, 
        0, 
        MPI_COMM_WORLD
    );

    if (rank == 0) {
        run_time = MPI_Wtime() - start_time;
        printf("MPI Matrix Multiplication Time: %f seconds\n", run_time);
    }

    MPI_Finalize();
    return 0;
}