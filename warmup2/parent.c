/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "parent.h"
#include "error.h"
#include "cs402.h"
#include "my402list.h"
#include "arrival_thread.h"
#include "token_thread.h"
#include "server_thread.h"
#include "monitor.h"
#include "stats.h"

void printEmulParams(EmulationParams *ep) {

	printf("Emulation Parameters:\n");
	printf("\tnumber to arrive = %d\n", ep->numPackets);

	if (ep->fileName == NULL) {
		printf("\tlambda = %f\n", ep->lambda);
		printf("\tmu = %.2f\n", ep->mu);
	}

	printf("\tr = %f\n", ep->r);
	printf("\tB = %d\n", ep->B);
	printf("\tP = %d\n", ep->P);

	if (ep->fileName != NULL) printf("\ttsfile = %s\n", ep->fileName);

	printf("\n");


}
void initThreadArgs(ThreadArgument *arrival_arg, ThreadArgument *deposit_arg,
				ThreadArgument *s1_arg, ThreadArgument *s2_arg, ThreadArgument *mon_arg,
				sigset_t *set, EmulationParams *ep) {

	My402List *q1 = (My402List *) calloc(1, sizeof(My402List));
	My402List *q2 = (My402List *) calloc(1, sizeof(My402List));
	My402List *packetList = (My402List *) calloc(1, sizeof(My402List));
	My402ListInit(q1);
	My402ListInit(q2);
	My402ListInit(packetList);

	pthread_mutex_t *token_m = (pthread_mutex_t*) calloc(1, sizeof(pthread_mutex_t));
	pthread_mutex_t *packetList_m = (pthread_mutex_t*) calloc(1, sizeof(pthread_mutex_t));
	pthread_cond_t *Q2NotEmpty = (pthread_cond_t*) calloc(1, sizeof(pthread_cond_t));
	pthread_cond_t *NoMorePackets = (pthread_cond_t*) calloc(1, sizeof(pthread_cond_t));
	*token_m = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	*packetList_m = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	*Q2NotEmpty = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	*NoMorePackets = (pthread_cond_t) PTHREAD_COND_INITIALIZER;


	arrival_arg->q1 = q1, arrival_arg->q2 = q2, arrival_arg->epPtr = ep; 
	arrival_arg->token_m = token_m;

	deposit_arg->q1 = q1, deposit_arg->q2 = q2, deposit_arg->epPtr = ep; 
	deposit_arg->token_m = token_m;

	s1_arg->q1 = q1, s1_arg->q2 = q2, s1_arg->epPtr = ep, s1_arg->token_m = token_m; 
	s1_arg->packetList = packetList, s1_arg->packetList_m = packetList_m, s1_arg->serverID = 0;

	s2_arg->q1 = q1, s2_arg->q2 = q2, s2_arg->epPtr = ep, s2_arg->token_m = token_m; 
	s2_arg->packetList = packetList, s2_arg->packetList_m = packetList_m, s2_arg->serverID = 1;

	arrival_arg->Q2NotEmpty = Q2NotEmpty, arrival_arg->NoMorePackets = NoMorePackets;
	deposit_arg->Q2NotEmpty = Q2NotEmpty, deposit_arg->NoMorePackets = NoMorePackets;
	s1_arg->Q2NotEmpty = Q2NotEmpty;
	s2_arg->Q2NotEmpty = Q2NotEmpty;
	mon_arg->set = set;
}

void initEmulParams(EmulationParams *ep) {

	const int NUM_PACKETS = 20, P = 3, B = 10;
	const double MU = 0.35, R = 1.5, LAMBDA = 1.0;

	// Values ignored if trace file provided: lambda, mu, P, numPackets
	// numPackets is read from the trace file and over-written later
	ep->lambda = LAMBDA;
	ep->mu = MU;
	ep->r = R; // 1/r = token inter-arrival-time; defaults to 10 seconds if 1/r greater than 10 seconds
	ep->B = B; // B = bucket size; permanent
	ep->P = P; 
	ep->numPackets = NUM_PACKETS;
	ep->fileName = NULL;
}

void updateEmulParams(EmulationParams *ep) {

	int enumParams = TRUE;
	PacketParams params = readInput(ep->fileName, enumParams, NULL);
	ep->numPackets = params.numPackets;
	ep->deterministic = FALSE;
	
}

double deltaTime(struct timeval *start, struct timeval *end) {

	const double CONVERSION_FACTOR = 1000.00;

	int sec = (int) end->tv_sec - (int) start->tv_sec;
	int microsec = (int) end->tv_usec - (int) start->tv_usec;
	double dTime = (sec * CONVERSION_FACTOR) + (microsec/CONVERSION_FACTOR);
	
	return dTime;

}

void runEmulation(EmulationParams *ep, sigset_t *set) {

	
	pthread_t sig_t, arrival_t, token_deposit_t, s1_t, s2_t;
    ThreadArgument arrival_arg, deposit_arg, s1_arg, s2_arg, mon_arg;
    struct timeval startTime, endTime;
    double dTime;

    initThreadArgs(&arrival_arg, &deposit_arg, &s1_arg, &s2_arg, &mon_arg, set, ep);
    void *(*arrivalFuncPtr)(void*);
    void *(*depositFuncPtr)(void*);
    void *(*serverFuncPtr)(void*);
    void *(*monitorFuncPtr)(void*);
    arrivalFuncPtr = &arrival;
    depositFuncPtr = &deposit;
    serverFuncPtr = &server;
    monitorFuncPtr = &sigMonitor;

    // set thread IDs for cancellation
    mon_arg.arrival_t = &arrival_t;
    mon_arg.deposit_t = &token_deposit_t;


    printEmulParams(ep);

    (void)gettimeofday(&startTime, NULL);
    dTime = deltaTime(&startTime, &startTime);
    ep->time_emul_start = startTime;

 	printf("%012.3fms: emulation begins\n", dTime);

	pthread_create(&sig_t, 0, monitorFuncPtr, (void *) &mon_arg);
	pthread_create(&arrival_t, 0, arrivalFuncPtr, (void *) &arrival_arg);
	pthread_create(&token_deposit_t, 0, depositFuncPtr, (void *) &deposit_arg);
	pthread_create(&s1_t, 0, serverFuncPtr, (void *) &s1_arg);
	pthread_create(&s2_t, 0, serverFuncPtr, (void *) &s2_arg);

	// pthread_join(sig_t, 0);
	pthread_join(arrival_t, 0);
	pthread_join(token_deposit_t, 0);
	// pthread_join(s1_t, 0);
	// pthread_join(s2_t, 0);
	pthread_cancel(sig_t);

	(void)gettimeofday(&endTime, NULL);
	dTime = deltaTime(&startTime, &endTime);
	ep->time_emul_end = endTime;

	printf("%012.3fms: emulation ends\n\n", dTime);

	printStats(&s1_arg);

}
void processArgs(int argc, char *argv[], EmulationParams *ep, char** fileNamePtr) {

	const int MAX_ARG_COUNT = 15;
	const int MIN_ARG_COUNT = 3;
	
	if (argc > MAX_ARG_COUNT) exitOnCmdError(TooManyArgs);
	else if (argc < MIN_ARG_COUNT || argc % 2 == 0) exitOnCmdError(UnknownCmd);
	else {

		int i;
		for (i = 1; i < argc - 1; i+=2) {

			char* command = argv[i];
			char* param = argv[i+1];
			char* end;

			if (strcmp("-lambda", command) == 0) ep->lambda = atoi(param);
			else if (strcmp("-mu", command) == 0) ep->mu = atoi(param);
			else if (strcmp("-r", command) == 0) ep->r = strtod(param, &end);
			else if (strcmp("-B", command) == 0) ep->B = atoi(param);
			else if (strcmp("-P", command) == 0) ep->P = atoi(param);
			else if (strcmp("-n", command) == 0) ep->numPackets = atoi(param);
			else if (strcmp("-t", command) == 0) {

				ep->fileName = param;
				exitOnFileError(ep->fileName);

			} 
			else exitOnCmdError(UnknownCmd);
		}

		if (ep->fileName == NULL) ep->deterministic = TRUE;
		else updateEmulParams(ep);
	}

}

int main(int argc, char* argv[]) {

	// TODO: thread return values // free memory
	
	char* fileName = NULL;
    EmulationParams ep;
    sigset_t set;
    endSimulation = FALSE;

    initEmulParams(&ep);
    processArgs(argc, argv, &ep, &fileName);

    // block SIGINT
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, 0);

    runEmulation(&ep, &set);
	
	 return(0);
}
