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

 

int getNumPackets(My402List *list, int servedOnly) {

	int count = 0;
	My402ListElem *elem = NULL;

	if (servedOnly == FALSE) count = list->num_members;
	else {

		for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list, elem)) {

				Packet *packet = (Packet *) elem->obj;
				if (packet->serveSuccess == servedOnly) count++;
			}
	}
	return count;
}

double getPacketDropProb() {

	return packetCount > 0 ? droppedPacketCount/(double) packetCount : 0;
}

double getTokenDropProb() {

	return tokenCount > 0 ? droppedTokenCount/(double)tokenCount : 0;
}

double getStdv(My402List *q, double avgSystime) {

	My402ListElem *elem = NULL;
	double variance = 0, dTime = 0, sum = 0;
	avgSystime = avgSystime*THOUSAND_FACTOR; // seconds to milliseconds

	for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {

		Packet *packet = (Packet *) elem->obj;
		if (packet->serveSuccess == TRUE) {
			dTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
			sum += (dTime - avgSystime) * (dTime - avgSystime);
		}
	}
	
	variance = sum / completedPacketCount;

	// avoid divide-by-zero error
	return variance > 0 ? sqrt(variance)/THOUSAND_FACTOR : 0; //milliseconds to seconds
}

double getAVgSysTime(My402List *q, TimeType t) {

	double totalTime = 0;
	double dTime = 0;
	My402ListElem *elem = NULL;

	for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {

		Packet *packet = (Packet *) elem->obj;
		if (t == MeasuredServiceTime && packet->serveSuccess == TRUE) {
			dTime = deltaTime(&packet->time_in_server, &packet->time_out_server);
		}
		else if (t == MeasuredSystemTime && packet->serveSuccess == TRUE) {
			dTime = deltaTime(&packet->time_arrival, &packet->time_out_server);
		}
		else if (t == MeasuredInterArrivalTime) dTime = packet->iaMeasured;
		
		totalTime += dTime;
		
	}
	// avoid divide-by-zero error
	double result;
	if (t == MeasuredInterArrivalTime) {
		result = allPacketCount > 0 ? (totalTime/allPacketCount)/THOUSAND_FACTOR : 0;
	}
	else {
		result = completedPacketCount > 0 ? (totalTime/completedPacketCount)/THOUSAND_FACTOR : 0; 
	}
	return  result;
	
}

double getAvgPacketNum(My402List *q, Facility f, double emulTime) {

	double totalTime = 0;
	My402ListElem *elem = NULL;
	const int SERVER_ONE = 1, SERVER_TWO = 2;

 	for (elem = My402ListFirst(q); elem != NULL; elem = My402ListNext(q, elem)) {

 		Packet *packet = (Packet *) elem->obj;
 		if (packet->serveSuccess == TRUE) {
	 		if (f == Q1) totalTime += deltaTime(&packet->time_in_q1, &packet->time_out_q1);

	 		else if (f == Q2) totalTime += deltaTime(&packet->time_in_q2, &packet->time_out_q2);

	 		else if (f == S1 && packet->serverID == SERVER_ONE) {
	 			
	 			totalTime += deltaTime(&packet->time_in_server, &packet->time_out_server);
	 		}
	 		else if (f == S2 && packet->serverID == SERVER_TWO) {

	 			totalTime += deltaTime(&packet->time_in_server, &packet->time_out_server);
	 		}
 		}
	}

	return emulTime > 0 ? (totalTime/emulTime) : 0;
}

void printStats(ThreadArgument *args) {

	
	double avgInterArrival = 0, avgServTime = 0, avgNumQ1 = 0, avgNumQ2 = 0, avgNumS1, avgNumS2;
	double avgSysTime, stdvSystime, tDropProb, pDropProp;
	

	double emulTime = deltaTime(&args->epPtr->time_emul_start, &args->epPtr->time_emul_end);
	My402List *packetList = args->packetList;
	completedPacketCount = getNumPackets(packetList, TRUE);
	allPacketCount = getNumPackets(packetList, FALSE);

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

	printf("\n\nDEBUG MODE - Packets generated: %d\n",allPacketCount);
	printf("DEBUG MODE - Packets served: %d\n\n",completedPacketCount);
}


