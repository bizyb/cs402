/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _WARMUP2_ARRIVAL_THREAD
#define _WARMUP2_ARRIVAL_THREAD

#include <sys/time.h>

#include "error.h"
#include "parent.h"

int packetCount;
int firstPacket;
struct timeval prevProcessingTime;
struct timeval prevArrivalTime;


typedef struct {

	int tokens;
	int interArrival;
	int serviceTime;
	int packetID;
	int serverID;
	double iaMeasured;
	double dServiceTime;
	double dSysTime;
	struct timeval time_arrival;
	struct timeval time_in_q1;
	struct timeval time_out_q1;
	struct timeval time_in_q2;
	struct timeval time_out_q2;
	struct timeval time_in_server;
	struct timeval time_out_server;
	int packetDropped;


} Packet;

typedef struct {

	int numPackets;
	int interArrival; // in microseconds
	int tokens;
	int serviceTime; // in microseconds

} PacketParams;


extern void matchPattern(char*, char*, ErrorType);
extern void validateLine(char* );
extern PacketParams getPacketParams(char *, char *, char* );
extern PacketParams getPacketParams_overload(char *);
extern PacketParams parseLine(char*, int);
extern PacketParams readInput(char*, int, ThreadArgument*);
extern Packet *getPacket(PacketParams);
extern PacketParams getDetParams(ThreadArgument *);
extern void enqueuePacketQ1(ThreadArgument *, Packet *);
extern void processPacket(ThreadArgument *, PacketParams);
extern void *arrival(void *);
#endif /*_WARMUP2_ARRIVAL_THREAD*/
