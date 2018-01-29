#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "parent.h"
#include "error.h"
#include "cs402.h"


double deltaTime(struct timeval *start, struct timeval *end) {

	const double CONVERSION_FACTOR = 1000.00;

	int sec = (int) end->tv_sec - (int) start->tv_sec;
	int microsec = (int) end->tv_usec - (int) start->tv_usec;
	double dTime = (sec * CONVERSION_FACTOR) + (microsec/CONVERSION_FACTOR);

	return dTime;

}

void runEmulation(EmulationParams *ep, char* fileName) {

	
	// pthread_t arrival_t, token_deposit_t, s1_t, s2_t;
 //    ThreadArgument arrival_arg, deposit_arg, server_arg;
    struct timeval startTime, endTime;
    double dTime;

    (void)gettimeofday(&startTime, NULL);
    dTime = deltaTime(&startTime, &startTime);

 	printf("%012.3fms: emulation begins\n", dTime);


	// pthread_create(&arrival_t, 0, arrival, getThreadArgs(Arrival, &arrival_arg));
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
void processArgs(int argc, char *argv[], EmulationParams *ep, char** fileNamePtr, FILE** inStreamPtr) {

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

		if (fileNamePtr == NULL) *inStreamPtr = stdin;
	}

}

int main(int argc, char* argv[]) {

	char* fileName = NULL;
    FILE* inStream = NULL;
    EmulationParams ep;

   
    processArgs(argc, argv, &ep, &fileName, &inStream);
    runEmulation(&ep, fileName);


	// signal handling
	// stat computation and printing
	// free memory



	 return(0);
}