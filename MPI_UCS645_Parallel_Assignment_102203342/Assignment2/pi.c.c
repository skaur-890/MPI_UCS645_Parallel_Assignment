#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define NO_OF_ITERATIONS 1000000

int main(int argc, char **argv)
{
    int rank, size;
    long long int circle_points = 0, square_points = 0, interval = 0;
    double x, y, d;
    double pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    while (interval < NO_OF_ITERATIONS / size)
    {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        d = x * x + y * y;

        if (d <= 1)
        {
            circle_points++;
        }

        square_points++;
        interval++;
    }

    long long int global_circle_points, global_square_points;
    MPI_Reduce(&circle_points, &global_circle_points, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&square_points, &global_square_points, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        pi = 4.0 * (double)global_circle_points / (double)global_square_points;

        printf("%f", pi);
    }

    return 0;
}