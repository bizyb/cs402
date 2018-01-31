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
int droppedPacketCount;
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

/*
	Perform regex matching for a particular transaction attribute. 
	If matching fails, throw the corresponding error message.
*/
extern void matchPattern(char*, char*, ErrorType);


/*
	Perform input line validation using regex pattern matching. Perform additional 
	validation on line length. If either one of the validation fails, print the 
	appropriate error message and exit the program. NB: Line validation is a more 
	holistic validation form.
*/
extern void validateLine(char* );


/*
	Return a packet params structure from an input file.
*/
extern PacketParams getPacketParams(char *, char *, char* );


/*
	Return a packet params structure for the first line of an input file.
*/
extern PacketParams getPacketParams_overload(char *);


/*
	Parse an input line by tokenizing it. Return a new PacketParams structure
	with the tokenized information.
*/
extern PacketParams parseLine(char*, int);


/*
	Process an input trace file one line at a time. If a file is being read 
	for the first time, return a PacketParams structure for the first line.
	Otherwise, process all subesequent lines and send each for packet processing. 
*/
extern PacketParams readInput(char*, int, ThreadArgument*);


/*
	Return a packet object containing information from a trace file. 
*/
extern Packet *getPacket(PacketParams);


/*
	Return deterministic packet parameters. 
*/
extern PacketParams getDetParams(ThreadArgument *);


/*
	Enqueue a packet in Q1. 
*/
extern void enqueuePacketQ1(ThreadArgument *, Packet *);


/*
	Perform the appropriate timestamping and logging for each packet. Enqueue the 
	packet as appropriate.
*/
extern void processPacket(ThreadArgument *, PacketParams);


/*
	Wait for packet arrival and call the appropriate functions to process each packet.
	If no more packets can enter the system, either because the stated number of 
	packets have already been processed or a Ctrl-C signal has been received, 
	terminate the thread. 
*/
extern void *arrival(void *);

#endif /*_WARMUP2_ARRIVAL_THREAD*/
