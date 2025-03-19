#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define N 10 // grid size
#define MAX_ITER 1000
#define THRESHOLD 0.01

void initialize_grid(double grid[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i == 0 || i == N - 1 || j == 0 || j == N - 1)
                grid[i][j] = 100.0; // boundary conditions
            else
                grid[i][j] = 0.0; // interior points
        }
    }
}

void print_grid(double grid[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%.1f ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int rank, num_procs, rows_per_proc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    rows_per_proc = N / num_procs;
    if (N % num_procs != 0)
    {
        if (rank == 0)
            printf("Error: grid rows must be divisble by number of processes.\n");
        MPI_Finalize();
        return -1;
    }

    double grid[N][N], local_grid[rows_per_proc + 2][N], new_grid[rows_per_proc + 2][N];

    if (rank == 0)
        initialize_grid(grid);

    // scatter the grid to processes
    MPI_Scatter(
        grid,
        rows_per_proc * N,
        MPI_DOUBLE,
        &local_grid[1][0],
        rows_per_proc * N,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    // Main computation loop
    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        if (rank > 0)
        {
            MPI_Send(
                &local_grid[1][0],
                N,
                MPI_DOUBLE,
                rank - 1,
                0,
                MPI_COMM_WORLD);
            MPI_Recv(
                &local_grid[0][0],
                N,
                MPI_DOUBLE,
                rank - 1,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        }
        if (rank < num_procs - 1)
        {
            MPI_Send(
                &local_grid[rows_per_proc][0],
                N,
                MPI_DOUBLE,
                rank + 1,
                0,
                MPI_COMM_WORLD);
            MPI_Recv(
                &local_grid[rows_per_proc + 1][0],
                N,
                MPI_DOUBLE,
                rank + 1,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
        }

        // computing new temp values
        double max_diff = 0.0;
        for (int i = 1; i <= rows_per_proc; i++)
        {
            for (int j = 1; j < N - 1; j++)
            {
                new_grid[i][j] = 0.25 * (local_grid[i - 1][j] + local_grid[i + 1][j] + local_grid[i][j - 1] + local_grid[i][j + 1]);
                double diff = fabs(new_grid[i][j] - local_grid[i][j]);
                if (diff > max_diff)
                    max_diff = diff;
            }
        }

        //swap grids
        for(int i=1; i<=rows_per_proc; i++){
            for(int j=0; j<N; j++){
                local_grid[i][j]=new_grid[i][j];
            }
        }

        double global_max_diff;
        MPI_Allreduce(
            &max_diff, 
            &global_max_diff,
            1, 
            MPI_DOUBLE, 
            MPI_MAX,
            MPI_COMM_WORLD);
        if (global_max_diff < THRESHOLD) break;
    }
 
    MPI_Gather(
        &local_grid[1][0],
        rows_per_proc*N,
        MPI_DOUBLE,
        grid,
        rows_per_proc*N,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );


    if(rank==0){
        printf("Final Heat Distribution.\n");
        print_grid(grid);
    }

    MPI_Finalize();
    return 0;
}