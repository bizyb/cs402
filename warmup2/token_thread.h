/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _WARMUP2_TOKEN_THREAD
#define _WARMUP2_TOKEN_THREAD

#include "parent.h"
#include "my402list.h"

int firstToken;
int tokenCount;
int avlblTokens;
int droppedTokenCount;
int endTokenDeposit;
struct timeval prevTokenProcTime;
struct timeval prevTokenArrivaltime;

/*
	Generate a token and add it to the token bucket. If a token has been generated
	successfully, update the global variables. If the bucket is full, drop the 
	token and output an appropriate message. 

	PRE: The global variable avlblTokens is between 0 and the size of the bucket
	POST: avlblTokens is one greater than before if token generation succeeds. 
		Otherwise, it stays the same.
*/
extern void generateToken(ThreadArgument *, double);


/*
	Dequeue a packet from Q1 and enqueue it onto Q2. Decrement the value of avlblTokens 
	by the number of tokens used by the packet. 

	PRE: The global variable avlblTokens is between 0 and the size of the bucket
	POST: avlblTokens is decremented by the token value of the packet
*/
extern void dequeueEnqueue(ThreadArgument *, My402ListElem *);


/*
	Attempt to transfer packets from Q1 to Q2 as long as the token bucket contains 
	enough tokens to transfer the packet at the head of Q1. 
*/ 
extern void transferPacket(ThreadArgument *);


/*
	Initiate timestamping of token generation and sleep for a specified amount of 
	time.
*/
extern void processToken(ThreadArgument *, int);


/*
	Return true if the maximum number of packets to be sered has been served.
*/
extern int maxPacketsReached(ThreadArgument *);


/*
	Monitor packet availability to generate tokens and initiate packet transfer. Broadcast
	the servers whenever Q2 becomes non-empty. If No more packets can enter the system and 
	Q1 is empty, immediately stop token generation and exit. 
*/
extern void *deposit(void *);

#endif /*_WARMUP2_TOKEN_THREAD*/
