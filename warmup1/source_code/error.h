/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#ifndef _WARMUP1_ERROR
#define _WARMUP1_ERROR



typedef enum {
	FileOpen,
	LongLine,
	Duplicate,
	HighAmount,
	HighBalance,
	LowBalance,
	UnknownFlag,
	UnknownAmount,
	Timestamp,
	Malloc,
	ListInsertion,
	TooManyArgs,
	UnknownCmd,
	TooFewArgs,
	FlagFormat,
	DateFormat,
	CurrencyFormat,
	DescFormat,
	EmptyLine,
	LineTooShort

} ErrorType;


/*
	Print data processing-related error message and exit program. 
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


#endif /*_WARMUP1_ERROR*/
