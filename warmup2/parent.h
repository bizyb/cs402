#ifndef _PARENT_H
#define _PARENT_H



typedef struct {

	double mu;
	double r;
	int B;
	int P;
	int n;
	int lambda;
	int numPackets;

} EmulationParams;

typedef enum {

	Arrival,
	TokenDeposit,
	Server1,
	Server2

} ThreadType;

typedef struct {


} ThreadArgument;

extern double deltaTime(struct timeval *, struct timeval *);
extern void runEmulation(EmulationParams *, char*);
extern void getThreadArgs(ThreadType, ThreadArgument*);
extern void processArgs(int, char *[], EmulationParams *, char**, FILE**);
#endif /*_PARENT_H*/