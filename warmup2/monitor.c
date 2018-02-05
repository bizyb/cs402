/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "parent.h"
#include "monitor.h"
#include "cs402.h"

void *sigMonitor(void * obj) {

	int sig;
	ThreadArgument *args = (ThreadArgument *) obj;
	sigset_t *set = args->set;

	while (TRUE) {

		sigwait(set, &sig);
		endSimulation = TRUE;
		// exit(1); 
		break;
	}

	return NULL;

}
