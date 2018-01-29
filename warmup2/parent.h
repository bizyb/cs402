#ifndef _PARENT_H
#define _PARENT_H

#include <pthread.h>
#include <sys/time.h>

#include "my402list.h"



typedef struct {

	double mu;
	double r;
	int B;
	int P;
	int lambda;
	int numPackets;
	int deterministic;
	char *fileName;

} EmulationParams;

typedef enum {

	Arrival,
	TokenDeposit,
	Server1,
	Server2

} ThreadType;

typedef struct {

	My402List *q1;
	My402List *q2;
	// My402List *packetList;
	EmulationParams *epPtr;
	// pthread_mutex_t q1_m;
	// pthread_mutex_t q2_m;
	pthread_mutex_t *token_m;
	// pthread_mutex_t packetList_m;

} ThreadArgument;

extern void initThreadArgs(ThreadArgument *, ThreadArgument *, ThreadArgument *,
							EmulationParams *);
extern void initEmulParams(EmulationParams *);
extern double deltaTime(struct timeval *, struct timeval *);
extern void runEmulation(EmulationParams *);
extern void processArgs(int, char *[], EmulationParams *, char**);
#endif /*_PARENT_H*/