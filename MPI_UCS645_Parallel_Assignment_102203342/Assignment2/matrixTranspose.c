#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 4

void print_matrix(int matrix[N][N], const char* label){
    printf("%s:\n", label);
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]){
    int rank, size;
    int matrix[N][N], transpose[N][N];
    int local_row[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(size!=N){
        if(rank==0){
            printf("this program requires exactly %d no. of processes\n", N);
        }
        MPI_Finalize();
    }

    if(rank==0){
        int count=1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = count++;
            }
        }
        print_matrix(matrix, "Original Matrix");
    }

    MPI_Scatter(
        matrix, 
        N,
        MPI_INT,
        local_row,
        N,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    int local_col[N];
    for(int i=0; i<N; i++){
        local_col[i]=local_row[i];
    }

    MPI_Gather(
        local_col,
        N,
        MPI_INT,
        transpose,
        N,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    if(rank==0){
        print_matrix(transpose, "Transposed Matrix");
    }

    MPI_Finalize();
    return 0;
}