#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

//Basic Send functions in MPI
// MPI_Send(
//     void* data,
//     int count,
//     MPI_Datatype datatype,
//     int destination,
//     int tag,
//     MPI_Comm communicator
// )

//Basic Receive functions in MPI
// MPI_Recv(
//     void* data,
//     int count,
//     MPI_Datatype datatype,
//     int source,
//     int tag,
//     MPI_Comm communicator,
//     MPI_Status* status
// )

int main(int argc, char** argv){

    MPI_Init(NULL, NULL);
    //rank
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //size
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // assuming 2 processors for this task
    if(world_size<5){
        fprintf(stderr,"World size must be greater than 5 for %s.\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 5);
    }

    int number;
    if(world_rank==0){
        number=-1;

        MPI_Send(
            &number,
            1,
            MPI_INT,
            1,
            0,
            MPI_COMM_WORLD
        );
    }
    else if(world_rank==1){
        MPI_Recv(
            &number,
            1,
            MPI_INT,
            0,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
        printf("process 1 received number %d from process 0.\n", number);
    }

    MPI_Finalize();
}