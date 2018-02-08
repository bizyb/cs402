/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "token_thread.h"
#include "arrival_thread.h"
#include "server_thread.h"
#include "cs402.h"
#include "parent.h"
#include "global.h"
#include "monitor.h"

void generateToken(ThreadArgument *args, double dTotal) {

	// pthread_mutex_lock(args->token_m);
	// pthread_cleanup_push(pthread_mutex_unlock, args->token_m);

	if (avlblTokens < args->epPtr->B) {
		if (avlblTokens == 0) {

			printf("%012.3fms: token t%d arrives, token bucket now has %d token\n", 
				dTotal, ++tokenCount, ++avlblTokens);
		}
		else {

			printf("%012.3fms: token t%d arrives, token bucket now has %d tokens\n", 
				dTotal, ++tokenCount, ++avlblTokens);
		}
	}
	else if (avlblTokens == args->epPtr->B ) {

		printf("%012.3fms: token t%d arrives, dropped\n", dTotal, ++tokenCount);
		droppedTokenCount++;

	}
	// pthread_cleanup_pop(1);	

}

void dequeueEnqueue(ThreadArgument *args, My402ListElem *elem) {

	struct timeval out_q1, in_q2;
	double dTime, dTotal;
	Packet *packet = (Packet*) elem->obj;

	My402ListUnlink(args->q1, elem);
	(void)gettimeofday(&out_q1, NULL);

	packet->time_out_q1 = out_q1;
	dTotal = deltaTime(&args->epPtr->time_emul_start, &packet->time_out_q1);
	dTime = deltaTime(&packet->time_in_q1, &packet->time_out_q1);

	// update tokens
	avlblTokens -= packet->tokens;
	printf("%012.3fms: p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d tokens \n",
		 dTotal, packet->packetID, dTime, avlblTokens);

	// update token deposit state
	My402ListAppend(args->q2, (void *) packet);
	(void)gettimeofday(&in_q2, NULL);

	packet->time_in_q2 = in_q2;
	dTotal = deltaTime(&args->epPtr->time_emul_start, &packet->time_in_q2);
	printf("%012.3fms: p%d enters Q2\n", dTotal, packet->packetID);

	pthread_cond_broadcast(args->Q2NotEmpty);

}

void transferPacket(ThreadArgument *args) {

	int done = FALSE;
	// pthread_mutex_lock(args->token_m);
	// pthread_cleanup_push(pthread_mutex_unlock, args->token_m);

	while (!done) {

		My402ListElem *elem = My402ListFirst(args->q1);
		if (elem == NULL) done = TRUE;
		else {
			Packet *packet = (Packet*) elem->obj;
			if (packet != NULL && packet->tokens <= avlblTokens) {
				dequeueEnqueue(args, elem);
			}
			else done = TRUE;
		}
		
	}
	// pthread_cleanup_pop(1);
}

void processToken(ThreadArgument *args, int tokenInterArrival) {

	struct timeval then, now;
	double dTime, dTotal;

	(void)gettimeofday(&then, NULL);

	if (firstToken == TRUE) {
		// account for the time elapsed since the start of the emulation and 
		// the first invocation of deposit()
		dTime = deltaTime(&args->epPtr->time_emul_start, &then) * THOUSAND_FACTOR;
		firstToken = FALSE;	
	}
	else {
		
		dTime = deltaTime(&prevTokenArrivaltime, &prevTokenProcTime) * THOUSAND_FACTOR;
		
	}
	if (dTime > 0) usleep(tokenInterArrival - (int) dTime);

	(void)gettimeofday(&now, NULL);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
	prevTokenArrivaltime = now;

	// printf("\n\nDEBUG point: mutex owner: %d\n\n", args->token_m->__data.__owner);
	// pthread_mutex_unlock(args->token_m);
	// printf("\n\nDEBUG point 2\n\n");
	pthread_mutex_lock(args->token_m);
	
	pthread_cleanup_push(pthread_mutex_unlock, args->token_m);
	
	if (endTokenDeposit == FALSE) generateToken(args, dTotal);
	transferPacket(args);
	// printf("\n\nDEBUG point 3\n\n");
	pthread_cleanup_pop(1);
	

	(void)gettimeofday(&prevTokenProcTime, NULL);
}

int maxPacketsReached(ThreadArgument *args) {

	if (packetCount == args->epPtr->numPackets) {

			pthread_mutex_lock(args->token_m);
			pthread_cleanup_push(pthread_mutex_unlock, args->token_m);

			if (args->q1->num_members == 0 && allPacketsArrived == TRUE) {endTokenDeposit = TRUE;}
			if (args->q2->num_members > 0) pthread_cond_broadcast(args->Q2NotEmpty);

			pthread_cleanup_pop(1);
		}

	return endTokenDeposit;
}

void *deposit(void * obj) {

	// printf("\n\nDEBUG: in depositing thread\n\n");

	firstToken = TRUE;
	endTokenDeposit = FALSE;
	tokenCount = 0;
	avlblTokens = 0;
	droppedTokenCount = 0;
	int tokenInterArrival;
	int maxRate = MAX_TOKEN_RATE * THOUSAND_FACTOR * THOUSAND_FACTOR;
	
	
	ThreadArgument *args = (ThreadArgument *) obj;
	tokenInterArrival = (1/args->epPtr->r) * THOUSAND_FACTOR * THOUSAND_FACTOR;

	if (tokenInterArrival > maxRate) tokenInterArrival = maxRate;

	while (endSimulation == FALSE) {
		// printf("\n\nDEBUG point 1\n\n");
		if (maxPacketsReached(args) == TRUE) break;
		processToken(args, tokenInterArrival);

	}
	// printf("\n\nDEBUG: exiting depositing thread\n\n");
	endSimulation = TRUE;
	// wake up the server threads
	pthread_cond_broadcast(args->Q2NotEmpty);
	return NULL;
}

