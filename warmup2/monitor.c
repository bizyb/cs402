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
	// printf("\n\nin signal thread\n\n");
	int sig;
	ThreadArgument *args = (ThreadArgument *) obj;
	sigset_t *set = args->set;

	// while (endSimulation == FALSE) {

		sigwait(set, &sig);
		endSimulation = TRUE;

		pthread_cancel(*args->arrival_t);
		pthread_cancel(*args->deposit_t);

		// break;
	// }
	// printf("\n\nexiting signal thread...\n\n");
	return NULL;

}
