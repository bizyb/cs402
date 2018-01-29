/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _WARMUP2_ARRIVAL_THREAD
#define _WARMUP2_ARRIVAL_THREAD

#include <sys/time.h>

#include "error.h"


typedef struct {

	int tokens;
	int iServiceTime;
	int packetID;
	int serverID;
	double dServiceTime;;
	double dSysTime;
	struct timeval time_arrival;
	struct timeval time_in_q1;
	struct timeval time_out_q1;
	struct timeval time_in_q2;
	struct timeval time_out_q2;
	struct timeval time_in_server;
	struct timeval time_out_server;


} Packet;

typedef struct {

	int numPackets;
	int interArrival;
	int tokens;
	int serviceTime;

} PacketParams;
extern void matchPattern(char*, char*, ErrorType);
extern void validateLine(char* );
extern PacketParams getPacketParams(char *, char *, char* );
extern PacketParams getPacketParams_overload(char *);
extern PacketParams parseLine(char*, int);
extern PacketParams readInput(char*, int);
extern void *arrival(void *);
#endif /*_WARMUP2_ARRIVAL_THREAD*/
