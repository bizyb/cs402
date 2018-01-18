/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#ifndef _WARMUP1_H
#define _WARMUP1_H

#include "cs402.h"

int lineNum;

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

typedef enum {

	DEPOSIT,
	WITHDRAWAL,
	NEGATIVE,
	POSITIVE,
} Flag;

typedef struct TransactionRecord {

	char* date;
	char* desc;
	int amount;
	int balance;
	int dateRaw;
	Flag flag;
	Flag balFlag;

} Transaction;


/*
	Read input data either from a file or stdin one line at a time. 
*/
extern int readInput(My402List*, char*, FILE*);

/*
	Evaluate a flag string and return the corresponding flag of 
	type Flag. 
*/
extern Flag getFlag(char*);

/*
	Perform type-casting on an amount string and return a value one 
	hundred times the float value so as to avoid future precision and 
	rounding errors. 
*/
extern int getAmount(char*);

/*
	Perform type-casting on a tokenized date string and return its 
	integral equivalent.
*/
extern int getRawDate(char*);

/*
	Print an error message and exit program. 
*/
extern void exitOnError(ErrorType);

/*
	Perform the appropriate type casting and formatting for each 
	transaction attribute and return a Transaction record.
*/
extern Transaction getTransaction(char*, char*, char*, char*);


/*
	Parse an input line by tokenizing it. Return a new Transaction record
	with the tokenized information.
*/
extern Transaction parseLine(char*);

/*
	Copy a transaction record from the stack to the heap and return
	the new recrod.
*/
extern Transaction* copyTransaction(Transaction, Transaction*);

/*
	Insert a Transaction record in sorted order.
*/
extern void insertTransaction(My402List*, Transaction);

/*
	Print a sorted list of all transactions.
*/
extern void printHistory(My402List*);

/*
	Given a Unix timestamp in seconds, return its corresponding date in the 
	format of Day Month Date Year.
*/
extern char* formatDate(int);

/*
	Format a currency value by defining its width and justifying the final value
	to the right. Return the formatted value as a string. 
*/
extern char* formatCurrency(int);

/*
	Insert a thousands separator into currency values about to be printed to
	stdout. If insertion is successfull, return TRUE. Return FALSE otherwsie. 
*/
extern int insertComma(char*);

/*
	Perform input line validation using regex pattern matching. Perform additional 
	validation on line length. If either one of the validation fails, print the 
	appropriate error message and exit the program. NB: Line validation is a more 
	holistic validation form. While it's unlikely that valid lines would escape 
	matching, redundant validations are still performed on the individual tokens
	in their corresponding functions.
*/
extern void validateLine(char*);

/*
	Parse command line arguments. If there are three command line arguments,,
	set the file stream to the third argument. Otherwise, se the file stream
	to stdin.
*/
extern void processArgs(int, char* [], char**, FILE**);

/*
	Remove any new line characters that was added into a string during 
	the string manipulation steps. NB: strtok() appends a new line character
	for data read in from stdin if certain columns do not conform to an expected
	length. This is true for the date column. Although we expect ten characters
	for the date, a single character is also valid but not properly tokenized. 

*/
extern char* cleanString(char*);

/*
	Free all the memory allocated for transaction recrods.
*/
extern void freeMemory(My402List*, My402ListElem*);

/*
	Compute the balance after the current transaction takes place
	for every record of transaction. 
*/
extern void computeBalance(My402List*);

/*
	Perform regex matching for a particular transaction attribute. 
	If matching fails, throw the corresponding error message.
*/
extern void matchPattern(char*, char*, ErrorType);
#endif /*_WARMUP1_H*/
