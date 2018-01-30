#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "token_thread.h"
#include "arrival_thread.h"
#include "cs402.h"
#include "parent.h"
#include "global.h"

void generateToken(ThreadArgument *args, double dTotal) {

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
	else if (avlblTokens == args->epPtr->B) {

		printf("%012.3fms: token t%d arrives, dropped\n", dTotal, ++tokenCount);
		dropCount++;

	}	

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

	My402ListAppend(args->q2, (void *) packet);
	(void)gettimeofday(&in_q2, NULL);

	packet->time_in_q2 = in_q2;
	// printf("packet->time_in_q2: %d\n", (int) packet->time_in_q2.tv_sec);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &packet->time_in_q2);
	printf("%012.3fms: p%d enters Q2\n", dTotal, packet->packetID);

}

void transferPacket(ThreadArgument *args) {

	int done = FALSE;
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
}


void *deposit(void * obj) {
	// TODO: this thread can only die if kill signal is received

	firstToken = TRUE;
	tokenCount = 0;
	avlblTokens = 0;
	dropCount = 0;
	int tokenInterArrival;
	struct timeval then, now;
	double dTime, dTotal;
	
	ThreadArgument *args = (ThreadArgument *) obj;
	tokenInterArrival = (1/args->epPtr->r) * THOUSAND_FACTOR * THOUSAND_FACTOR;

	while (tokenCount < 3) {

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

		pthread_mutex_lock(args->token_m);
		generateToken(args, dTotal);
		transferPacket(args);

		pthread_cond_broadcast(args->Q2NotEmpty);

		pthread_mutex_unlock(args->token_m);

		(void)gettimeofday(&prevTokenProcTime, NULL);


	}
	

	return NULL;
}
