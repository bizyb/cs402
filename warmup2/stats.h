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

extern double getAvgPacketNum(My402List *, Facility, double);
extern void printStats(ThreadArgument *);
#endif /*_WARMUP2_STATS_THREAD*/