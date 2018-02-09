/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _MONITOR_H
#define _MONITOR_H

#include "cs402.h"
#include "parent.h"
#include "my402list.h"

int endSimulation;
int signalReceived;


/*
	Remove packets left behind in the queue upon signal receipt
	and place them in the packet archive.
*/
extern void removePackets(ThreadArgument *, My402List *, int);

/*
	Monitor Ctrl-C (SIGINT) signal and update the global variables 
	when the signal is received. Initiate packet removal if signal 
	received.
*/
extern void *sigMonitor(void *);
#endif /*_MONITOR_H*/
