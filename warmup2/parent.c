#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "parent.h"
#include "error.h"
#include "cs402.h"

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

	 return(0);
}