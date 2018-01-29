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

typedef struct {

	Arrival,
	TokenDeposit,
	Server1,
	Server2
	
} ThreadType;

extern void processArgs(int, char *[], EmulationParams *, char**, FILE**);
#endif /*_PARENT_H*/