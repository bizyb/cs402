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

extern int getNumPackets(My402List *);
extern double getPacketDropProb();
extern double getTokenDropProb();
extern double getAVgSysTime(My402List *, TimeType);
extern double getStdv(My402List *, double);
extern double getAvgPacketNum(My402List *, Facility, double);
extern void printStats(ThreadArgument *);

#endif /*_WARMUP2_STATS_THREAD*/