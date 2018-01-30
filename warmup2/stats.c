#include <stdio.h>

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

	return -1.733;
}
double getTokenDropProb() {

	return dropCount/(double)tokenCount;
}

double getStdv(My402List *p) {

	return -1.732;
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
		// printf("dTime: %f\n", dTime);
		totalTime += dTime;
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

 		else if (f == S1 && packet->serverID + 1 == SERVER_ONE) {
 			totalTime += deltaTime(&packet->time_in_server, &packet->time_out_server);
 		}
 		else if (f == S2 && packet->serverID + 1 == SERVER_TWO) {
 			totalTime += deltaTime(&packet->time_in_server, &packet->time_out_server);
 		}
	}

	return (totalTime/emulTime)/THOUSAND_FACTOR;
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

	stdvSystime = getStdv(packetList);
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

