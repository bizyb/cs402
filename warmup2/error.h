/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#ifndef _WARMUP2_ERROR
#define _WARMUP2_ERROR



typedef enum {
	
	
	TooManyArgs,
	UnknownCmd,
	TooFewArgs,
	InvalidEmulParams,
	NumPackets,
	InterArrival,
	Tokens,
	ServiceTime,
	LongLine,
	EmptyLine,
	Malloc,
	ListInsertion

} ErrorType;


/*
	Print miscellaneous error messages. 
*/
extern void exitOnError(ErrorType);

/*
	Print command line argument-related error message and exit program.
*/
extern void exitOnCmdError(ErrorType);

/*
	Print file processing-related error message and exit program. 
*/
extern void exitOnFileError(char*);


#endif /*_WARMUP2_ERROR*/
