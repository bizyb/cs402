#include <stdio.h>
#include <unistd.h>

#include "token_thread.h"
#include "cs402.h"
#include "parent.h"
#include "global.h"

void generateToken(ThreadArgument *args, double dTotal) {

	if (avlblTokens < args->epPtr->B) {
		if (avlblTokens == 0) {

			printf("%012.3fms: token t%d arrives, token bucket now has %d token\n", 
				dTotal, ++tokenCount, ++avlblTokens);
		}
		else {

			printf("%012.3fms: token t%d arrives, token bucket now has %d tokens\n", 
				dTotal, ++tokenCount, ++avlblTokens);

		}
	}
	else if (avlblTokens == args->epPtr->B) {

		printf("%012.3fms: token t%d arrives, dropped\n", dTotal, ++tokenCount);

	}	

}

void *deposit(void * obj) {
	
	firstToken = TRUE;
	tokenCount = 0;
	avlblTokens = 0;
	int tokenInterArrival;
	struct timeval then, now;
	double dTime, dTotal;
	
	ThreadArgument *args = (ThreadArgument *) obj;
	tokenInterArrival = (1/args->epPtr->r) * THOUSAND_FACTOR * THOUSAND_FACTOR;

	while (TRUE) {

		(void)gettimeofday(&then, NULL);
		if (firstToken == TRUE) {
			// account for the time elapsed since the start of the emulation and 
			// the first invocation of deposit()
			dTime = deltaTime(&args->epPtr->time_emul_start, &then) * THOUSAND_FACTOR;
			firstToken = FALSE;
			// usleep(tokenInterArrival - (int) dTime);	
		}
		else {
			dTime = deltaTime(&prevTokenArrivaltime, &prevTokenProcTime) * THOUSAND_FACTOR;
			
		}
		if (dTime > 0) usleep(tokenInterArrival - (int) dTime);

		(void)gettimeofday(&now, NULL);
		dTotal = deltaTime(&args->epPtr->time_emul_start, &now);
		prevTokenArrivaltime = now;

		pthread_mutex_lock(args->token_m);
		generateToken(args, dTotal);
		// transferPacket(args);
		pthread_mutex_unlock(args->token_m);

		(void)gettimeofday(&prevTokenProcTime, NULL);


	}
	

	return NULL;
}
