
#ifndef _WARMUP1_ERROR
#define _WARMUP1_ERROR



typedef enum {
	FileOpen,
	LongLine,
	// MalformedLine,
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
extern void exitOnErrorCmd(ErrorType);

/*
	Print file processing-related error message and exit program. 
*/
extern void exitOnErrorFile(char*);


#endif /*_WARMUP1_ERROR*/