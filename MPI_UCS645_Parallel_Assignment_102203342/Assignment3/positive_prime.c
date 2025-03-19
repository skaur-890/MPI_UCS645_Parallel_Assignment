#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define MAX_VALUE 100 // Maximum number to check for primes
#define MASTER 0

int is_prime(int num)
{
    if (num < 2)
        return 0;
    for (int i = 2; i <= sqrt(num); i++)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == MASTER)
    {
        int candidate = 2;
        int workers = size - 1;
        MPI_Status status;
        int received, source;

        // Send initial tasks to workers
        for (int i = 1; i <= workers && candidate <= MAX_VALUE; i++)
        {
            MPI_Send(&candidate, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            candidate++;
        }

        // Receive results and assign new tasks
        while (candidate <= MAX_VALUE)
        {
            MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            source = status.MPI_SOURCE;

            if (received > 0)
            {
                printf("Prime: %d\n", received);
            }

            // Send a new number to check
            MPI_Send(&candidate, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
            candidate++;
        }

        // Collect remaining results from workers
        for (int i = 1; i <= workers; i++)
        {
            MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (received > 0)
            {
                printf("Prime: %d\n", received);
            }
        }

        // Send termination signal to all workers
        int stop_signal = -1;
        for (int i = 1; i <= workers; i++)
        {
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    { 
        // Worker processes
        while (1)
        {
            int test_number;
            MPI_Recv(&test_number, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (test_number == -1)
                break; // Exit condition

            int result = is_prime(test_number) ? test_number : -1;
            MPI_Send(&result, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}