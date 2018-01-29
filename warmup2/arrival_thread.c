#include <stdio.h>

#include "parent.h"
#include "cs402.h"
#include "arrival_thread.h"

void *arrival(void * obj) {

	ThreadArgument *args = (ThreadArgument *) obj;
	printf("in arrival thread. emul start time: %d\n", (int)args->epPtr->time_emul_start.tv_sec);
	return NULL;
}