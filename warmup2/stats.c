/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <stdio.h>
#include <math.h>

#include "stats.h"
#include "cs402.h"
#include "parent.h"
#include "my402list.h"
#include "arrival_thread.h"
#include "token_thread.h"
#include "global.h"


int getNumPackets(My402List *q) {

	return q->num_members;
}

double getPacketDropProb() {

	return droppedPacketCount/(double) packetCount;
}
double getTokenDropProb() {

	return droppedTokenCount/(double)tokenCount;
}

double getStdv(My402List *q, double avgSystime) {

	My402ListElem *elem = NULL;
	double variance = 0, dTime = 0, sum = 0;
	avgSystime = avgSystime*THOUSAND_FACTOR; // seconds to milliseconds

	for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {

		Packet *packet = (Packet *) elem->obj;
		dTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
		sum += (dTime - avgSystime) * (dTime - avgSystime);
	}
	
	variance = sum / getNumPackets(q);

	return sqrt(variance)/THOUSAND_FACTOR; //milliseconds to seconds
}
double getAVgSysTime(My402List *q, TimeType t) {

	double totalTime = 0;
	double dTime;
	My402ListElem *elem = NULL;

	for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {

		Packet *packet = (Packet *) elem->obj;
		if (t == MeasuredServiceTime) {
			dTime = deltaTime(&packet->time_in_server, &packet->time_out_server);
		}
		else if (t == MeasuredSystemTime) {
			dTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
		}
		else if (t == MeasuredInterArrivalTime) dTime = packet->iaMeasured;
		
		totalTime += dTime;
		// printf("\n\ndTime: %f\n\n", dTime);
	}
	return (totalTime/getNumPackets(q))/THOUSAND_FACTOR;

}

double getAvgPacketNum(My402List *q, Facility f, double emulTime) {

	double totalTime = 0;
	My402ListElem *elem = NULL;
	const int SERVER_ONE = 1, SERVER_TWO = 2;

 	for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {

 		Packet *packet = (Packet *) elem->obj;
 		if (f == Q1) totalTime += deltaTime(&packet->time_in_q1, &packet->time_out_q1);
 		else if (f == Q2) totalTime += deltaTime(&packet->time_in_q2, &packet->time_out_q2);

 		else if (f == S1 && packet->serverID == SERVER_ONE) {
 			
 			totalTime += deltaTime(&packet->time_in_server, &packet->time_out_server);
 		}
 		else if (f == S2 && packet->serverID == SERVER_TWO) {

 			totalTime += deltaTime(&packet->time_in_server, &packet->time_out_server);
 		}
	}

	return (totalTime/emulTime);
}

void printStats(ThreadArgument *args) {

	double avgInterArrival, avgServTime, avgNumQ1, avgNumQ2, avgNumS1, avgNumS2;
	double avgSysTime, stdvSystime, tDropProb, pDropProp;
	

	double emulTime = deltaTime(&args->epPtr->time_emul_start, &args->epPtr->time_emul_end);
	My402List *packetList = args->packetList;

	
	avgInterArrival = getAVgSysTime(packetList, MeasuredInterArrivalTime);

	avgNumQ1 = getAvgPacketNum(packetList, Q1, emulTime);
	avgNumQ2 = getAvgPacketNum(packetList, Q2, emulTime);
	avgNumS1 = getAvgPacketNum(packetList, S1, emulTime);
	avgNumS2 = getAvgPacketNum(packetList, S2, emulTime);

	avgServTime = getAVgSysTime(packetList, MeasuredServiceTime);
	avgSysTime = getAVgSysTime(packetList, MeasuredSystemTime);

	stdvSystime = getStdv(packetList, avgSysTime);
	tDropProb = getTokenDropProb();
	pDropProp = getPacketDropProb();

	printf("Statistics:\n\n");

	printf("\taverage packet inter-arrival time = %.6f\n", avgInterArrival);
	printf("\taverage packet service time = %.6f\n\n", avgServTime);

	printf("\taverage number of packets in Q1 = %.6f\n", avgNumQ1);
	printf("\taverage number of packets in Q2 = %.6f\n", avgNumQ2);
	printf("\taverage number of packets in S1 = %.6f\n", avgNumS1);
	printf("\taverage number of packets in S2 = %.6f\n\n", avgNumS2);

	printf("\taverage time a packet spent in system = %.6f\n", avgSysTime);
	printf("\tstandard deviation for time spent in system = %.6f\n\n", stdvSystime);

	printf("\ttoken drop probability = %.6f\n", tDropProb);
	printf("\tpacket drop probability = %.6f\n\n", pDropProp);
}


