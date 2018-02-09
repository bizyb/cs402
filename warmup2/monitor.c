/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "parent.h"
#include "monitor.h"
#include "server_thread.h"
#include "cs402.h"



void removePackets(ThreadArgument *args, My402List *q, int queueNum) {

	struct timeval now;
	double dTotal;
	My402ListElem *elem = NULL;
	queueNum++;

	if (q->num_members > 0) {
		for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {
		
			Packet *packet = (Packet *) elem->obj;
			archivePacket(args, packet, FALSE);

			(void)gettimeofday(&now, NULL);
			dTotal = deltaTime(&args->epPtr->time_emul_start, &now);

			printf("%012.3fms: p%d removed from Q%d\n", dTotal, packet->packetID, queueNum);
		}

		My402ListUnlinkAll(q);
	}
	
}

void *sigMonitor(void * obj) {
	
	struct timeval now;
	double dTotal;
	// int sig;
	signalReceived = FALSE;
	ThreadArgument *args = (ThreadArgument *) obj;
	sigset_t *set = args->set;

	// sigwait(set, &sig); //POSIX standard-compliant
	sigwait(set); //UNIX

	(void)gettimeofday(&now, NULL);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &now);

	
	endSimulation = TRUE;
	signalReceived = TRUE;

	pthread_cancel(*args->arrival_t);
	pthread_cancel(*args->deposit_t);

	// Do packet removal atomically. Note that the order of the mutex locks
	// ensures that the servers go to sleep if they're done serving their current
	// packets so that we can remove the packets without any interruption. The
	// signal broadcast at the end then ensures that any sleeping thread is woken
	// up and exits its thread normally. 
	
	pthread_mutex_lock(args->token_m);
	pthread_mutex_lock(args->packetList_m);

	printf("%012.3fms: SIGINT caught, no new packets or tokens will be allowed\n",
	 		dTotal);
	removePackets(args, args->q1, 0);
	removePackets(args, args->q2, 1);

	pthread_mutex_unlock(args->packetList_m);
	pthread_mutex_unlock(args->token_m);

	//signal the server threads in case they're currently 
	//waiting for a signal	
	pthread_cond_broadcast(args->Q2NotEmpty);
			
	return NULL;

}
