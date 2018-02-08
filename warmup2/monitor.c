/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parent.h"
#include "monitor.h"
#include "cs402.h"


void *sigMonitor(void * obj) {
	
	// int sig;
	signalReceived = FALSE;
	ThreadArgument *args = (ThreadArgument *) obj;
	sigset_t *set = args->set;

	// sigwait(set, &sig); //POSIX standard-compliant

	sigwait(set); //UNIX 
	endSimulation = TRUE;
	signalReceived = TRUE;

	pthread_cancel(*args->arrival_t);
	pthread_cancel(*args->deposit_t);

	//signal the server threads in case they're currently 
	//waiting for a signal	
	pthread_cond_broadcast(args->Q2NotEmpty);
			
	return NULL;

}
