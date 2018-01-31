/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _WARMUP2_SERVER_THREAD
#define _WARMUP2_SERVER_THREAD

#include "parent.h"

/*
	Perform packet transmission by dequeuing a target packet from Q2 and serving
	it by going to sleep. Then timestamp the packet's departure from the server 
	and add it to a list of served packets. 

	PRE: pthread_mutex is currently locked by the server thread and Q2 contains a 
	packet in its queue that can be transmitted immediately.

	POST: All pthread_mutex_t objects are unlocked 
*/
extern void transmitPacket(ThreadArgument *);


/*
	Run the server thread in an infinite loop until no more packets can enter the
	system and Q2 is empty. If Q2 not-empty signal is received, lock the mutex and 
	transimit the packet. Unlock the mutex after the packet has been served. 	
*/
extern void *server(void *);

#endif /*_WARMUP2_SERVER_THREAD*/
