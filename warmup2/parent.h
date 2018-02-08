/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _PARENT_H
#define _PARENT_H

#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

#include "my402list.h"

extern int lineNum;
extern int endEmulsig;

typedef struct {

	double mu;
	double lambda;
	double r;
	int B;
	int P;
	int numPackets;
	int deterministic;
	char *fileName;
	struct timeval time_emul_start;
	struct timeval time_emul_end;

} EmulationParams;

typedef enum {

	Arrival,
	TokenDeposit,
	Server1,
	Server2

} ThreadType;

typedef struct {

	int serverID;
	sigset_t *set;
	My402List *q1;
	My402List *q2;
	My402List *packetList;
	EmulationParams *epPtr;
	pthread_mutex_t *token_m;
	pthread_mutex_t *packetList_m;
	pthread_cond_t *Q2NotEmpty;
	pthread_cond_t *NoMorePackets;
	pthread_t *arrival_t;
	pthread_t *deposit_t;

} ThreadArgument;

/*
	Print initial emulation parameters. 	
*/
extern void printEmulParams(EmulationParams *);


/*
	Initialize thread arguments to be passed to each thread. Each thread
	receives a custom set of data structures that are shared globally among
	all other threads. 
*/
extern void initThreadArgs(ThreadArgument *, ThreadArgument *, ThreadArgument *,
							ThreadArgument *, ThreadArgument *, sigset_t *,
							EmulationParams *);


/*
	Initialize emulation parameters with default values.
*/
extern void initEmulParams(EmulationParams *);


/*
	Update emulation parameters using data read in from a trace file.
*/
extern void updateEmulParams(EmulationParams *);


/*
	Compute the time elapsed between two points and return the 
	difference in milliseconds.	
*/
extern double deltaTime(struct timeval *, struct timeval *);


/*
	Remove packets left behind in the queue upon signal receipt
	and place them in the packet archive.
*/
extern void removePackets(ThreadArgument *, My402List *);


/*
	Initiate emulation by creating one thread for packet arrival, 
	on thread for token genration, one thread for server 1, and one 
	thread for server 2. Wait for each thread to finish.
*/
extern void runEmulation(EmulationParams *, sigset_t *);


/*
	Process command line arguments. Raise the appropriate error for each
	malformed command and terminate program. 
*/
extern void processArgs(int, char *[], EmulationParams *, char**);

#endif /*_PARENT_H*/
