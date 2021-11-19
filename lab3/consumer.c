//+
//    ELEC 377,  Lab 3
//
//  consumer.c
//
// The purpose of the consumer is to transfer a signle charector at a time from the queue to the output. It uses a nested loop with a flag to indicate when it has retrieved a byte from the buffer and the program exits when the number of producers is zero
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
	int c, numProd;
    
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
	
    // put your code here...


	getMutex(&sharedPtr->lock);
	numProd=sharedPtr->prodCount; //get producer count. Must lock before and unlock after
	releaseMutex(&sharedPtr->lock);

	int charRead =TRUE;//initialize flag to true

	//+
	// countinues to execute while there are producers remaining and while the flag is true. Uses nested while within main while loop. Must lock before and unlock after. While count is greater than zero, get charector from queue and store in variable 'c'. Then update out_output, decrement count variable, and set flag. After releasing lock, use putchar with 'c' variable as input
	//-
	while(numProd>0 && charRead==TRUE){
		charRead=FALSE;
		while(charRead==FALSE && numProd>0){
			getMutex(&sharedPtr->lock);
			if(sharedPtr->count>0){
				c=sharedPtr->buffer[sharedPtr->out_output];
				sharedPtr->out_output=(sharedPtr->out_output+1)%BUFFSIZE;
				sharedPtr->count--;
				charRead=TRUE;
			}else{
				numProd=sharedPtr->prodCount;
			}
			releaseMutex(&sharedPtr->lock);
		}
		putchar(c);
	}
    
	return 0;
}


