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

	pthread_mutex_unlock(args->token_m);

	return packet;
}

void archivePacket(ThreadArgument *args, Packet *packet) {

	pthread_mutex_lock(args->packetList_m);
	My402ListAppend(args->packetList, (void *)packet);
	pthread_mutex_unlock(args->packetList_m);
}

void logActivity(ThreadArgument *args, Packet *packet, Activity a, double dTime, double dTotal) {

	pthread_mutex_lock(args->token_m);

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

	pthread_mutex_unlock(args->token_m);

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
	usleep(packet->serviceTime);

	// Finish serving the packet and archive it
	(void)gettimeofday(&now, NULL);
	packet->time_out_server = now;
	dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
	dTime = deltaTime(&packet->time_in_server, &packet->time_out_server);
	sysTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
	packet->dSysTime = sysTime;
	logActivity(args, packet, ServerExit, dTime, dTotal);
	archivePacket(args, packet);
}

int allPacketsServed(ThreadArgument *args) {

	int exitThread = FALSE;
	if (packetCount == args->epPtr->numPackets) {

			pthread_mutex_lock(args->token_m);
			if (args->q1->num_members == 0 && args->q2->num_members == 0) {

				exitThread = TRUE;
			}
			pthread_mutex_unlock(args->token_m);
	}
	return exitThread;
}

void *server(void *obj) {

	
	ThreadArgument *args = (ThreadArgument *) obj;
	
	while(endSimulation == FALSE) {
		
		if (allPacketsServed(args) == TRUE) break;

		pthread_cond_wait(args->Q2NotEmpty, args->token_m);

		// check again that q2 is not empty
		while (args->q2->num_members > 0) transmitPacket(args);
		
		pthread_mutex_unlock(args->token_m);
	}
	return NULL;
}
