/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <pthread.h>


#include "parent.h"
#include "error.h"
#include "cs402.h"
#include "global.h"
#include "arrival_thread.h"
#include "token_thread.h"
#include "monitor.h"

int lineNum = 1;

void matchPattern(char* line, char* pattern, ErrorType e) {

    regex_t patternBuffer;
    int formatFail;

    formatFail  = regcomp(&patternBuffer, pattern, REG_EXTENDED);
    formatFail = regexec(&patternBuffer, line, 0, NULL, 0);
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

    if (lineNum > 1) {
	    matchPattern(line, interArrivalPattern, InterArrival);
	    matchPattern(line, tokensPattern, Tokens);
	    matchPattern(line, sTimePattern, ServiceTime);
	}
	else matchPattern(line, numPacketsPattern, NumPackets);

    //verify line width
    int i;
    for(i = 0; i < BUFFERSIZE; i++) {

        if (line[i] == '\n') break;
    }

    if (i + 1 > MAX_LINE) exitOnError(LongLine);

}

PacketParams getPacketParams(char *interArrival, char *tokens, char* serviceTime) {

	// TODO: multiplying by THOUSAND_FACTOR may cause an overflow; this would be incorrect since the 
	// original input could have been valid;

	int iArrival, iTokens, iSrvTime;

	iArrival = atoi(interArrival) * THOUSAND_FACTOR;
	iTokens = atoi(tokens);
	iSrvTime = atoi(serviceTime) * THOUSAND_FACTOR;

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

    char* delim = " \t\n";
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

PacketParams readInput(char* fileName, int enumParams, ThreadArgument *args) {

    char buffer [BUFFERSIZE];
    FILE* file;
    lineNum = 1;
    PacketParams params;

    if (fileName != NULL) {

        file = fopen(fileName, "r");
        if (file == NULL)  exitOnFileError(fileName);

    }

    pthread_cleanup_push(fclose, file);
    while (fgets(buffer, BUFFERSIZE,  file) != NULL && endSimulation == FALSE) {
 
        if (buffer[0] != '\n') {

            params = parseLine((char* ) &buffer, enumParams);
            if (enumParams == TRUE) break;
            if (lineNum > 1 ) processPacket(args, params);
            lineNum++;
        }
        else exitOnError(EmptyLine);
        
    }
    pthread_cleanup_pop(1);

    return params;

}

Packet *getPacket(PacketParams params) {

	Packet *packet = (Packet*) calloc(1, sizeof(Packet));

	packet->tokens = params.tokens;
	packet->interArrival = params.interArrival;
	packet->serviceTime = params.serviceTime;
	packet->packetID = ++packetCount;

	return packet;

}

PacketParams getDetParams(ThreadArgument *args) {

	const int MAX_SECONDS = 10 * THOUSAND_FACTOR * THOUSAND_FACTOR;
	double interArrival, serviceTime;
	int tokens;


	tokens = args->epPtr->P;
	serviceTime = 1/(args->epPtr->mu) * THOUSAND_FACTOR * THOUSAND_FACTOR;
	interArrival = 1/(args->epPtr->lambda) * THOUSAND_FACTOR * THOUSAND_FACTOR;

	if (serviceTime > MAX_SECONDS) serviceTime = MAX_SECONDS;
	if (interArrival > MAX_SECONDS) interArrival = MAX_SECONDS;

	PacketParams params = {-1, interArrival, tokens, serviceTime};
	return params;


}
void enqueuePacketQ1(ThreadArgument * args, Packet *packet) {

	struct timeval in_q1;
	double dTotal;

	My402ListAppend(args->q1, (void *) packet);
	(void)gettimeofday(&in_q1, NULL);
	packet->time_in_q1 = in_q1;
	dTotal = deltaTime(&args->epPtr->time_emul_start, &packet->time_in_q1);
	printf("%012.3fms: p%d enters Q1\n", dTotal, packet->packetID);

	
	if (args->q1->num_members == 1 && packet->tokens <= avlblTokens) {

		My402ListElem *elem = My402ListFirst(args->q1);
		dequeueEnqueue(args, elem);

		pthread_cond_broadcast(args->Q2NotEmpty);

	}

}

int getSleepTime(ThreadArgument * args, PacketParams params) {

	int sleepTime;
	double dTime;
	struct timeval then;

	(void)gettimeofday(&then, NULL);

	if (firstPacket == TRUE) {
		// account for the time elapsed since the start of the emulation and 
		// the first invocation of processPacket()
		dTime = deltaTime(&args->epPtr->time_emul_start, &then) * THOUSAND_FACTOR;
	}

	else dTime = deltaTime(&prevArrivalTime, &prevProcessingTime) * THOUSAND_FACTOR;

	sleepTime = params.interArrival - (int) dTime;

	if (sleepTime < 0) sleepTime = 0;

	return sleepTime;
}

void setInterArrivalTime(ThreadArgument * args, Packet *packet, double *dTime, double *dTotal) {

	struct timeval now;

	(void)gettimeofday(&now, NULL);
	packet->time_arrival = now;

	if (firstPacket == TRUE) {
		*dTime = deltaTime(&args->epPtr->time_emul_start, &now);
		firstPacket = FALSE;
	}
	else *dTime = deltaTime(&prevArrivalTime, &packet->time_arrival);

	*dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
	prevArrivalTime = now;
	packet->iaMeasured = *dTime;

}

void processPacket(ThreadArgument * args, PacketParams params) {

	double dTime, dTotal;
	Packet *packet = getPacket(params);

	usleep(getSleepTime(args, params));
	setInterArrivalTime(args, packet, &dTime, &dTotal);

	pthread_mutex_lock(args->token_m);
	pthread_cleanup_push(pthread_mutex_unlock, args->token_m);
	if (packet->tokens > args->epPtr->B) {

		printf("%012.3fms: p%d arrives, needs %d tokens, inter-arrival time = %.3fms, dropped\n", 
			dTotal, packet->packetID, packet->tokens, dTime);
		droppedPacketCount++;

	}
	else {
		printf("%012.3fms: p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n", 
				dTotal, packet->packetID, packet->tokens, dTime);

		enqueuePacketQ1(args, packet);
		
	}
	pthread_cleanup_pop(1);

	(void)gettimeofday(&prevProcessingTime, NULL);

}

void processDeterministic(ThreadArgument *args) {

	int numPackets = args->epPtr-> numPackets;
	PacketParams detParams;

	detParams = getDetParams(args);
	int i = 0;
	while (i < numPackets && endSimulation == FALSE) {

		processPacket(args, detParams);
		i++;	
	}

}
void *arrival(void * obj) {

	packetCount = 0;
	firstPacket = TRUE;
	droppedPacketCount = 0;
	allPacketsArrived = FALSE;

	ThreadArgument *args = (ThreadArgument *) obj;

	if (args->epPtr->deterministic == TRUE) processDeterministic(args);
	else (void) readInput(args->epPtr->fileName, FALSE, args);
	
	allPacketsArrived = TRUE;
	maxPacketsReached(args);

	// pthread_mutex_lock(args->token_m);
	// printf("\n\n arrival thread exiting\n\n");
	// pthread_mutex_unlock(args->token_m);
	return NULL;
}

