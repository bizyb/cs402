#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "parent.h"

void processArgs(int argc, char *argv[], EmulationParams *ep, char** fileNamePtr, FILE** inStreamPtr) {

	// const int MAX_ARG_COUNT = 15;
	// const int MIN_ARG_COUNT = 3;
	// raise too many args error
	// [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]
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
		else if (strcmp("-t", command) == 0) *fileNamePtr = param;
		// else raise unknown command error
	}

	if (fileNamePtr == NULL) *inStreamPtr = stdin;

	// raise too few args error

}

int main(int argc, char* argv[]) {

	char* fileName = NULL;
    FILE* inStream = NULL;
    EmulationParams ep;

    processArgs(argc, argv, &ep, &fileName, &inStream);

	 return(0);
}