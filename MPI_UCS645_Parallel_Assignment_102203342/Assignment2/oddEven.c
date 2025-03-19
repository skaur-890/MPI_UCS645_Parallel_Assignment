#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void even_odd_sort(int *local_array, int local_size, int global_size, int rank, int num_procs){
    int phase, partner, temp;
    MPI_Status status;

    for(phase=0; phase<global_size; phase++){
        if(phase%2==0){ //even phase
            if(rank%2==0)
                partner=rank+1;
            else
                partner=rank-1;
        }
        else{ //odd phase
            if(rank%2==0)
                partner=rank-1;
            else
                partner=rank+1;
        }

        if(partner>=0 && partner<num_procs){
            int received; 
            MPI_Sendrecv(
                &local_array[0],
                1,
                MPI_INT,
                partner,
                0,
                &received,
                1,
                MPI_INT,
                partner,
                0,
                MPI_COMM_WORLD,
                &status
            );

            if((rank<partner && local_array[0] > received) || (rank>partner && local_array[0]<received)){
                temp=local_array[0];
                local_array[0]=received;
                received=temp;
            }

            MPI_Sendrecv(
                &received,
                1,
                MPI_INT,
                partner,
                0,
                &local_array[0],
                1,
                MPI_INT,
                partner,
                0,
                MPI_COMM_WORLD,
                &status
            );
        }
    }
}

int main(int argc, char *argv[]){
    int rank, num_procs;
    int global_size=8;
    int local_array[1];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(num_procs!=global_size){
        if(rank==0){
            printf("Error: the number of processes must be equal to the number of elemnts.\n");
        }
        MPI_Finalize();
        return 1;
    }

    int global_array[]={9,7,3,5,1,8,6,2};

    
    MPI_Scatter(
        global_array,
        1,
        MPI_INT,
        local_array,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    if(rank==1){

        printf("Inital Array: ");
        for(int i=0; i<global_size; i++){
            printf("%d ", global_array[i]);
        }
        printf("\n");
    }
    
    even_odd_sort(
        local_array,
        1,
        global_size,
        rank,
        num_procs
    );
    
    MPI_Gather(
        local_array,
        1,
        MPI_INT,
        global_array,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    
    
    if(rank==0){
        printf("sorted array: ");
        for(int i=0; i<global_size; i++){
            printf("%d ", global_array[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}