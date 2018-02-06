/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _MONITOR_H
#define _MONITOR_H

#include "cs402.h"

int endSimulation;
int signalReceived;


/*
	Monitor for Ctrl-C (SIGINT) signal and update the global variable 
	when the signal is received.
*/
extern void *sigMonitor(void *);
#endif /*_MONITOR_H*/
