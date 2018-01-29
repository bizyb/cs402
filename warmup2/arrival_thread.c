#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "parent.h"
#include "error.h"
#include "cs402.h"
#include "global.h"
#include "arrival_thread.h"

int lineNum = 1;

void matchPattern(char* line, char* pattern, ErrorType e) {

    regex_t patternBuffer;
    int formatFail;

    formatFail  = regcomp(&patternBuffer, pattern, REG_EXTENDED);
    fprintf(stdout, "compilation: %d\n", formatFail);
    formatFail = regexec(&patternBuffer, line, 0, NULL, 0);
    fprintf(stdout, "matching: %d\n", formatFail);
    regfree(&patternBuffer);

    if (formatFail) exitOnError(e);

}

void validateLine(char* line) {

    //verify formatting
    // Note that the pattens are additive. If the previous patten fails, the next
    // one never gets executed since the program exits. 

    char* numPacketsPattern = "^([0-9]{1,10}\n)";
    char* interArrivalPattern =  "^([0-9]{1,10})\\s+";
    char* tokensPattern = "^([0-9]{1,10})\\s+([0-9]{1,10})\\s+";
    char* sTimePattern = "^([0-9]{1,10})\\s+([0-9]{1,10})\\s+([0-9]{1,10})\n";

   
    matchPattern(line, numPacketsPattern, NumPackets);
    if (lineNum > 1) {

	    matchPattern(line, interArrivalPattern, InterArrival);
	    matchPattern(line, tokensPattern, Tokens);
	    matchPattern(line, sTimePattern, ServiceTime);
	}

    //verify line width
    int i;
    for(i = 0; i < BUFFERSIZE; i++) {

        if (line[i] == '\n') break;
    }

    if (i + 1 > MAX_LINE) exitOnError(LongLine);

}

PacketParams getPacketParams(char *interArrival, char *tokens, char* serviceTime) {

	int iArrival, iTokens, iSrvTime;

	iArrival = atoi(interArrival);
	iTokens = atoi(tokens);
	iSrvTime = atoi(serviceTime);

	// TODO: what's the minimum number of tokens required?

	if (iArrival < 1 || iArrival > MAX_INT) exitOnError(InterArrival);
	if (iTokens < 1 || iTokens > MAX_INT) exitOnError(Tokens);
	if (iSrvTime < 1 || iSrvTime > MAX_INT) exitOnError(ServiceTime);

	PacketParams params = {-1, iArrival, iTokens, iSrvTime};
	return params;

}

PacketParams getPacketParams_overload(char *numPackets) {

	int iNumPackets;

	iNumPackets = atoi(numPackets);
	if (iNumPackets < 1 || iNumPackets > MAX_INT) exitOnError(NumPackets);

	PacketParams params = {iNumPackets, -1, -1, -1};
	return params;
}

PacketParams parseLine(char* line, int enumParams) {

    // #TODO: the delimiter can be a space or tab character
    char* delim = "\\s";
    char *numPackets, *interArrival, *tokens, *serviceTime;
    PacketParams params;

    validateLine(line);
    
    if (lineNum == 1 || enumParams == TRUE) {

    	numPackets = strtok(line, delim);
    	params = getPacketParams_overload(numPackets);

    }
    else {

	    interArrival = strtok(line, delim);
	    tokens = strtok(NULL, delim);
	    serviceTime = strtok(NULL, delim);
	    params = getPacketParams(interArrival, tokens, serviceTime);
	}
	return params; 
}

PacketParams readInput(char* fileName, int enumParams) {

    char buffer [BUFFERSIZE];
    FILE* file;
    lineNum = 1;
    PacketParams params;

    if (fileName != NULL) {

        file = fopen(fileName, "r");
        if (file == NULL)  exitOnFileError(fileName);

    }

    while (fgets(buffer, BUFFERSIZE,  file) != NULL) {

        if (buffer[0] != '\n') {

            params = parseLine((char* ) &buffer, enumParams);
            if (enumParams == TRUE) break;
            lineNum++;

        }
        else exitOnError(EmptyLine);
        
    }
    if (file != NULL) fclose(file);

    return params;

}

void *arrival(void * obj) {

	ThreadArgument *args = (ThreadArgument *) obj;
	printf("in arrival thread. emul start time: %d\n", (int)args->epPtr->time_emul_start.tv_sec);
	return NULL;
}

