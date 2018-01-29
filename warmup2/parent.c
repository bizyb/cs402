#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "parent.h"
#include "error.h"
#include "cs402.h"
#include "my402list.h"

void initThreadArgs(ThreadArgument *arrival_arg, ThreadArgument *deposit_arg,
				ThreadArgument *server_arg, EmulationParams *ep) {

	My402List *q1 = (My402List *) calloc(1, sizeof(My402List));
	My402List *q2 = (My402List *) calloc(1, sizeof(My402List));
	My402ListInit(q1);
	My402ListInit(q2);

	pthread_mutex_t *token_m = (pthread_mutex_t*) calloc(1, sizeof(pthread_mutex_t));
	// My402List *packetList = (My402List *) calloc(1, sizeof(My402List));

	arrival_arg->q1 = q1, arrival_arg->q2 = q2, arrival_arg->epPtr = ep, arrival_arg->token_m = token_m;



}

void initEmulParams(EmulationParams *ep) {

	const int LAMBDA = 1, NUM_PACKETS = 20, P = 3, B = 10;
	const double MU = 0.35, R = 1.5; 

	// Values ignored if trace file provided: lambda, mu, P, numPackets
	ep->lambda = LAMBDA;
	ep->mu = MU;
	ep->r = R; // 1/r = token inter-arrival-time; permanent
	ep->B = B; // B = bucket size; permanent
	ep->P = P; 
	ep->numPackets = NUM_PACKETS;
	ep->fileName = NULL;
}

double deltaTime(struct timeval *start, struct timeval *end) {

	const double CONVERSION_FACTOR = 1000.00;

	int sec = (int) end->tv_sec - (int) start->tv_sec;
	int microsec = (int) end->tv_usec - (int) start->tv_usec;
	double dTime = (sec * CONVERSION_FACTOR) + (microsec/CONVERSION_FACTOR);

	return dTime;

}

void runEmulation(EmulationParams *ep) {

	
	// pthread_t arrival_t, token_deposit_t, s1_t, s2_t;
    ThreadArgument arrival_arg, deposit_arg, server_arg;
    struct timeval startTime, endTime;
    double dTime;

    initThreadArgs(&arrival_arg, &deposit_arg, &server_arg, ep);


    (void)gettimeofday(&startTime, NULL);
    dTime = deltaTime(&startTime, &startTime);

 	printf("%012.3fms: emulation begins\n", dTime);


	// pthread_create(&arrival_t, 0, arrival, (void *) &arrival_arg);
 //    pthread_create(&token_deposit_t, 0, deposit, getThreadArgs(TokenDeposit, &deposit_arg));
 //    pthread_create(&s1_t, 0, server, getThreadArgs(Server1, &server_arg));
 //    pthread_create(&s2_t, 0, server, getThreadArgs(Server2, &server_arg));

	// pthread_join(arrival_t, 0);
	// pthread_join(token_deposit_t, 0);
	// pthread_join(s1_t, 0);
	// pthread_join(s2_t, 0);

	(void)gettimeofday(&endTime, NULL);
	dTime = deltaTime(&startTime, &endTime);

	printf("%012.3fms: emulation ends\n", dTime);

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

				*fileNamePtr = param;
				exitOnFileError(*fileNamePtr);

			} 
			else exitOnCmdError(UnknownCmd);
		}

		if (fileNamePtr == NULL) ep->deterministic = TRUE;
	}

}

int main(int argc, char* argv[]) {

	char* fileName = NULL;
    EmulationParams ep;

    initEmulParams(&ep);
    processArgs(argc, argv, &ep, &fileName);

    ep.fileName = fileName;
    runEmulation(&ep);


	// signal handling
	// stat computation and printing
	// free memory



	 return(0);
}