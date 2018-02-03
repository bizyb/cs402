#include <signal.h>
#include <stdio.h>

#include "monitor.h"
#include "cs402.h"

void *sigMonitor(void * obj) {

	int endEmulsig = FALSE;
	int sig;
	sigset_t *set = (sigset_t *) obj;

	while (TRUE) {

		sigwait(set, &sig);
		endEmulsig = TRUE;
		break;
	}

	return NULL;

}