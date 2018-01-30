#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


#include "cs402.h"
#include "my402list.h"
#include "parent.h"
#include "server_thread.h"

void *server(void *obj) {

	ThreadArgument *args = (ThreadArgument *) obj;
	while(TRUE) {

		//wait for wake up call here
		// once wake up received, lock q2 with token_m
		//remove packet from q2, timestamp, serverID
		//unlock token_m

		//serve the packet by sleeping
		//lock packetList
		//timestamp ejection
		//append packet to packetList
		//unlock packetList

	}
	return NULL;
}