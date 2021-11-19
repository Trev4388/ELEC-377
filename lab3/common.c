//+ 	ELEC 377, Lab 3
//
//   common.c contains routines to be used from both the
//   producer, and the  consumer
//   Mutual Exclusion routines will be here
//-

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <asm/system.h>


#include "common.h"

#define FALSE 0
#define TRUE 1


int test_and_set(int * lock){
    return __cmpxchg(lock,0,1,4);
}

//+ getMutex function to lock before entering critical section of code
void getMutex(int *  lock){
	// this should not return until it has mutual exclusion. Note that many versions of 
	// this will probobly be running at the same time.
	while(test_and_set(lock));
	}

//+ releaseMutex function to release lock after exiting critical section
void releaseMutex(int * lock){
	// set the mutex back to initial state so that somebody else can claim it
	*lock =0;
}

