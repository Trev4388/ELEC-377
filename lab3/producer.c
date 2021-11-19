//+
//	ELEC 377,  Lab 3
//
//  producer.c
//
// The producer begins by incrementing the number of producers that are accessing the shared buffer. The program must make calls to getMutex and releaseMutex since the producer count is a shared resource. The producer reads charectors using getchar() until it reaches the end of the file. The program uses getMutex before entering the critical section and checks to see if there is room in the queue. If there is, it adds the charector to the queue and releases the mutex. When the producer is successful in adding to the buffer, a flag is set. 
//
//-

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"

#define FALSE 0
#define TRUE 1


int main (int argc, char *argv[]){

	// initialize the shared memory, load in the initial array's, spawn the worker
	// processes.

	key_t   key;
	struct shared    *sharedPtr;
	int shmid;
	int c,stored;

    
	/*	 Shared memory init 	*/
	key = ftok(".", 'S');
	if((shmid = shmget(key, MEMSIZE, IPC_CREAT|0666)) == -1 ){
		if( (shmid = shmget(key, MEMSIZE, 0)) == -1){
			printf("Error allocating shared memory. \n");
			exit(1);
		}
	}

	// now map the region..
	if((int)(sharedPtr = (struct shared *) shmat(shmid, 0, 0)) == -1){
	    printf("Couldn't map the memory into our process space.\n");
	    exit(1);
	}
	
    // put your code here..
	
	getMutex(&sharedPtr->lock);
	(sharedPtr->prodCount)++; //increment producer count (lock and unlock before accessing shared element)
	releaseMutex(&sharedPtr->lock);

	//while loop countinues until no more charectors to read
	while((c=getchar())!=EOF){ 
		stored = FALSE; //set flag to false
		while(stored==FALSE){
			getMutex(&sharedPtr->lock);//lock before entering critical section 
			if((sharedPtr->count)<BUFFSIZE){ //ensure count is less than buffersize 
				sharedPtr->buffer[sharedPtr->in_input]=c; //store c in buffer
				sharedPtr->in_input=(sharedPtr->in_input+1)%BUFFSIZE; //set in_input
				sharedPtr->count++; //increment count
				stored=TRUE; //set flag to true
			}
			releaseMutex(&sharedPtr->lock); //release lock
		}
	}
	getMutex(&sharedPtr->lock);
	(sharedPtr->prodCount)--; /decrement prodCount
	releaseMutex(&sharedPtr->lock);

	return 0;
}


