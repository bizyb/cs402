#ifndef _WARMUP2_TOKEN_THREAD
#define _WARMUP2_TOKEN_THREAD

#include "parent.h"
#include "my402list.h"

int firstToken;
int tokenCount;
int avlblTokens;
struct timeval prevTokenProcTime;
struct timeval prevTokenArrivaltime;

extern void dequeueEnqueue(ThreadArgument *, My402ListElem *); 
extern void generateToken(ThreadArgument *, double);
extern void *deposit(void *);
#endif /*_WARMUP2_TOKEN_THREAD*/