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

void transmitPacket(ThreadArgument *args) {

	struct timeval then, now, out_q2;
	double dTime, dTotal, sysTime;

	//dequeue packet
	My402ListElem *elem = My402ListFirst(args->q2);
	Packet *packet = (Packet *) elem->obj;

	My402ListUnlink(args->q2, elem);
	(void)gettimeofday(&out_q2, NULL);
	

	pthread_mutex_unlock(args->token_m);

	packet->time_out_q2 = out_q2;

	dTime = deltaTime(&packet->time_in_q2, &packet->time_out_q2);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &packet->time_out_q2);


	pthread_mutex_lock(args->token_m);
	printf("%012.3fms: p%d leaves Q2, time in Q2 = %.3fms\n", 
			dTotal, packet->packetID, dTime);
	pthread_mutex_unlock(args->token_m);

	(void)gettimeofday(&then, NULL);
	dTotal = deltaTime(&args->epPtr->time_emul_start, &then);
	packet->time_in_server = then;

	int serverID = args->serverID+1;
	packet->serverID = serverID;
	int serviceTime = packet->serviceTime/THOUSAND_FACTOR;

	pthread_mutex_lock(args->token_m);
	printf("%012.3fms: p%d begins service at S%d, requesting %dms of service\n", 
			dTotal, packet->packetID, serverID, serviceTime);
	pthread_mutex_unlock(args->token_m);

	// service the packet
	usleep(packet->serviceTime);
	(void)gettimeofday(&now, NULL);
	packet->time_out_server = now;

	dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
	dTime = deltaTime(&packet->time_in_server, &packet->time_out_server);
	sysTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
	packet->dSysTime = sysTime;
	

	pthread_mutex_lock(args->token_m);
	printf("%012.3fms: p%d departs from S%d, service time = %.3fms, time in system =  %.3fms\n",
	 dTotal, packet->packetID, serverID, dTime, packet->dSysTime);
	pthread_mutex_unlock(args->token_m);



	pthread_mutex_lock(args->packetList_m);
	// printf("\n\nserver thread packet archived\n\n");
	My402ListAppend(args->packetList, (void *)packet);
	pthread_mutex_unlock(args->packetList_m);
}

void *server(void *obj) {

	int exitThread = FALSE;
	ThreadArgument *args = (ThreadArgument *) obj;
	
	while(endSimulation == FALSE) {

		if (packetCount == args->epPtr->numPackets) {

			pthread_mutex_lock(args->token_m);

			if (args->q1->num_members == 0 && args->q2->num_members == 0) {

				exitThread = TRUE;
			}
			pthread_mutex_unlock(args->token_m);
		}
		
		if (exitThread == TRUE) break;

		// printf("\n\nserver thread waiting for signal\n\n");
		pthread_cond_wait(args->Q2NotEmpty, args->token_m);
		// printf("\n\nserver thread signal received\n\n");
		// check again that q2 is not empty
		while (args->q2->num_members > 0) {

			transmitPacket(args);

		}
		pthread_mutex_unlock(args->token_m);
	}
	pthread_exit(NULL);
	return NULL;
}
