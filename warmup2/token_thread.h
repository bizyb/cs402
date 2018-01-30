#ifndef _WARMUP2_TOKEN_THREAD
#define _WARMUP2_TOKEN_THREAD

#include "parent.h"

int firstToken;
int tokenCount;
int avlblTokens;
struct timeval prevTokenProcTime;
struct timeval prevTokenArrivaltime;

extern void generateToken(ThreadArgument *, double);
extern void *deposit(void *);
#endif /*_WARMUP2_TOKEN_THREAD*/