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

	// TODO: multiplying by THOUSAND_FACTOR may cause overflow; this would be incorrect since the 
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

    while (fgets(buffer, BUFFERSIZE,  file) != NULL) {

        if (buffer[0] != '\n') {

            params = parseLine((char* ) &buffer, enumParams);
            if (enumParams == TRUE) break;
            if (lineNum > 1) processPacket(args, params);
            lineNum++;

        }
        else exitOnError(EmptyLine);
        
    }
    if (file != NULL) fclose(file);

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

void processPacket(ThreadArgument * args, PacketParams params) {

	// TODO: negative sleep time skipping is not yet tested
			// 	5. go back to sleep for the interArrival time of the next packet 
			// 		the sleep time is the next packets interArrival time minus 
			// 		the time it took to create the current packet and queue it; 
			// 		queueing could take a while since the mutex could be locked by 
			// 		some other thread; 
			// 		If the time difference is negative, sleep time should be zero,

	struct timeval then, now;
	double dTime, dTotal;

	(void)gettimeofday(&then, NULL);
	if (firstPacket == TRUE) {
		// account for the time elapsed since the start of the emulation and 
		// the first invocation of processPacket()
		dTime = deltaTime(&args->epPtr->time_emul_start, &then) * THOUSAND_FACTOR;
		usleep(params.interArrival - (int) dTime);
		
	}
	else {
		dTime = deltaTime(&prevArrivalTime, &prevProcessingTime) * THOUSAND_FACTOR;
		if (dTime > 0) usleep(params.interArrival - (int) dTime);
	}
	
	(void)gettimeofday(&now, NULL);
	

	Packet *packet = getPacket(params);
	packet->time_arrival = now;

	if (firstPacket == TRUE) {
		dTime = deltaTime(&args->epPtr->time_emul_start, &now);
		firstPacket = FALSE;
	}
	else dTime = deltaTime(&prevArrivalTime, &packet->time_arrival);

	dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
	prevArrivalTime = now;
	packet->iaMeasured = dTime;

	pthread_mutex_lock(args->token_m);
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
	pthread_mutex_unlock(args->token_m);

	(void)gettimeofday(&prevProcessingTime, NULL);

}

void *arrival(void * obj) {
	
	packetCount = 0;
	firstPacket = TRUE;
	droppedPacketCount = 0;
	PacketParams detParams;
	ThreadArgument *args = (ThreadArgument *) obj;
	int numPackets = args->epPtr-> numPackets;

	if (args->epPtr->deterministic == TRUE) {

		detParams = getDetParams(args);
		int i = 0;
		while (i < numPackets) {
			processPacket(args, detParams);
			i++;
		}
	}
	else {
		int enumParams = FALSE;
		(void) readInput(args->epPtr->fileName, enumParams, args);
	}
	
	pthread_exit(NULL);
	return NULL;
}

