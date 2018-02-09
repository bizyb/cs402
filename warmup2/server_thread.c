/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


#include "cs402.h"
#include "my402list.h"
#include "parent.h"
#include "global.h"
#include "arrival_thread.h"
#include "server_thread.h"
#include "monitor.h"


Packet *dequeuePacketQ2(ThreadArgument *args) {

	struct timeval out_q2;

	My402ListElem *elem = My402ListFirst(args->q2);
	Packet *packet = (Packet *) elem->obj;

	My402ListUnlink(args->q2, elem);
	(void)gettimeofday(&out_q2, NULL);
	packet->time_out_q2 = out_q2;

	return packet;
}

void archivePacket(ThreadArgument *args, Packet *packet, int served) {

	pthread_mutex_lock(args->packetList_m);

	if (served == TRUE) packet->serveSuccess = TRUE;
	My402ListAppend(args->packetList, (void *)packet);
	
	pthread_mutex_unlock(args->packetList_m);
}

void logActivity(ThreadArgument *args, Packet *packet, Activity a, double dTime, double dTotal) {

	if (a == Q2Exit) {

		printf("%012.3fms: p%d leaves Q2, time in Q2 = %.3fms\n", 
				dTotal, packet->packetID, dTime);
	}
	else if (a == ServerStart) {
		int serviceTime = packet->serviceTime/THOUSAND_FACTOR;
		printf("%012.3fms: p%d begins service at S%d, requesting %dms of service\n", 
			dTotal, packet->packetID, packet->serverID, serviceTime);

	}
	else if (a == ServerExit) {

		printf("%012.3fms: p%d departs from S%d, service time = %.3fms, time in system =  %.3fms\n",
	 		dTotal, packet->packetID, packet->serverID, dTime, packet->dSysTime);
	}

}
void transmitPacket(ThreadArgument *args) {

	struct timeval then, now;
	double dTime, dTotal, sysTime;

	// dequeue the packet from Q2
	Packet *packet = dequeuePacketQ2(args);
	dTime = deltaTime(&packet->time_in_q2, &packet->time_out_q2);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &packet->time_out_q2);
	logActivity(args, packet, Q2Exit, dTime, dTotal);

	// service the packet at the current server
	(void)gettimeofday(&then, NULL);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &then);
	packet->time_in_server = then;
	packet->serverID = args->serverID+1;
	logActivity(args, packet, ServerStart, dTime, dTotal);
	pthread_mutex_unlock(args->token_m);
	usleep(packet->serviceTime);

	// Finish serving the packet and archive it
	(void)gettimeofday(&now, NULL);
	packet->time_out_server = now;
	dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
	dTime = deltaTime(&packet->time_in_server, &packet->time_out_server);
	sysTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
	packet->dSysTime = sysTime;

	pthread_mutex_lock(args->token_m);
	logActivity(args, packet, ServerExit, dTime, dTotal);
	pthread_mutex_unlock(args->token_m);

	archivePacket(args, packet, TRUE);
	
}

int allPacketsServed(ThreadArgument *args) {

	int exitThread = FALSE;\
	// printf("\n\nDEBUG 1\n\n");
	if (packetCount == args->epPtr->numPackets) {
		// printf("\n\nDEBUG 2\n\n");
		if (args->q1->num_members == 0 && args->q2->num_members == 0 
			&& allPacketsArrived == TRUE) {
			exitThread = TRUE;
			endSimulation = TRUE;
			// printf("\n\nDEBUG 3\n\n");
		}
	// printf("\n\nDEBUG: args->q1->num_members: %d\n\n",args->q1->num_members);
	// printf("\n\nDEBUG: args->q2->num_members: %d\n\n",args->q2->num_members);
	// printf("\n\nDEBUG: allPacketsArrived: %d\n\n",allPacketsArrived);
	// printf("\n\nDEBUG 4\n\n");
	}
	return exitThread;
}

void *server(void *obj) {

	int mutexLocked = FALSE;
	ThreadArgument *args = (ThreadArgument *) obj;
	
	while(endSimulation == FALSE) {

		pthread_mutex_lock(args->token_m);
		// printf("\n\nDEBUG a: serverID: %d\n\n", args->serverID+1);
		while (args->q2->num_members == 0 && endSimulation == FALSE && allPacketsServed(args) == FALSE) { 
			// printf("\n\nDEBUG b: serverID: %d\n\n", args->serverID+1);
			pthread_cond_wait(args->Q2NotEmpty, args->token_m);
			// printf("\n\nDEBUG c: serverID: %d\n\n", args->serverID+1);
		}
		// transmitPacket returns with the mutex unlocked
		// do not service any packets from the queue if SIGINT has been issued
		if (args->q2->num_members > 0 && signalReceived == FALSE) transmitPacket(args);
		else pthread_mutex_unlock(args->token_m);

		if (allPacketsServed(args) == TRUE || signalReceived == TRUE) {
			// it's possible that the other thread is still waiting so signal it
			pthread_cond_signal(args->Q2NotEmpty);
			// mutexLocked = TRUE;
			break;
		}

				


		// if (allPacketsServed(args) == TRUE || signalReceived == TRUE) {
		// 	if (mutexLocked == TRUE) {
		// 		pthread_mutex_unlock(args->token_m);
		// 		mutexLocked = FALSE;
		// 	}
		// 	break;
		// }
		// if (mutexLocked == FALSE) {
		// 	pthread_mutex_lock(args->token_m);
		// 	mutexLocked = TRUE;
		// }

		// while (args->q2->num_members > 0) { 
		// 	if (signalReceived == FALSE) {
		// 		if (mutexLocked == FALSE) {
		// 			pthread_mutex_lock(args->token_m);
		// 			mutexLocked = TRUE;
		// 		}
		// 		transmitPacket(args);
		// 		mutexLocked = FALSE;
		// 	}
		// 	else mutexLocked = TRUE;
		// }
		// if (mutexLocked == FALSE) {
		// 	pthread_mutex_lock(args->token_m);
		// 	mutexLocked = TRUE;
		// }
		// if (allPacketsServed(args) == FALSE || signalReceived == FALSE) {
		// 	printf("\n\nDEBUG 5\n\n");
		// 	pthread_cond_wait(args->Q2NotEmpty, args->token_m);
		// }
		// mutexLocked = TRUE;

	}
	if (mutexLocked == TRUE) pthread_mutex_unlock(args->token_m);
	// printf("\n\nDEBUG d: exiting serverID: %d\n\n", args->serverID+1);
	return NULL;
}


