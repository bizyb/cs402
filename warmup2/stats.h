/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _WARMUP2_STATS_THREAD
#define _WARMUP2_STATS_THREAD
#include "parent.h"

typedef enum {

	Q1,
	Q2,
	S1,
	S2

} Facility;

typedef enum {

	MeasuredServiceTime,
	MeasuredSystemTime,
	MeasuredInterArrivalTime

} TimeType;

/*
	Return number of packets successfuly transmitted.
*/
extern int getNumPackets(My402List *);


/*
	Return packet drop probability.
*/
extern double getPacketDropProb();


/*
	Return token drop probability.
*/
extern double getTokenDropProb();


/*
	Compute and return the standard deviation of the system time spent
	by the packets.
*/
extern double getStdv(My402List *, double);


/*
	Return the average system time spent by the packets.
*/
extern double getAVgSysTime(My402List *, TimeType);


/*
	Return the average packet number in Q1, Q2, S1, or S2 computing as the amount
	of time spent by all the packets at each facility devided by the total emulation
	time.
*/
extern double getAvgPacketNum(My402List *, Facility, double);


/*
	Print a formatted statistics of the emulation.
*/
extern void printStats(ThreadArgument *);

#endif /*_WARMUP2_STATS_THREAD*/
