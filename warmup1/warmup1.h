/******************************************************************************/
/* Important CSCI 402 usage information:                                     */
/*                                                                           */
/* This fils is part of CSCI 402 programming assignments at USC.             */
/*         53616c7465645f5f2e8d450c0c5851acd538befe33744efca0f1c4f9fb5f      */
/*         3c8feabc561a99e53d4d21951738da923cd1c7bbd11b30a1afb11172f80b      */
/*         984b1acfbbf8fae6ea57e0583d2610a618379293cb1de8e1e9d07e6287e8      */
/*         de7e82f3d48866aa2009b599e92c852f7dbf7a6e573f1c7228ca34b9f368      */
/*         faaef0c0fcf294cb                                                  */
/* Please understand that you are NOT permitted to distribute or publically  */
/*         display a copy of this file (or ANY PART of it) for any reason.   */
/* If anyone (including your prospective employer) asks you to post the code,*/
/*         you must inform them that you do NOT have permissions to do so.   */
/* You are also NOT permitted to remove or alter this comment block.         */
/* If this comment block is removed or altered in a submitted file, 20 points*/
/*         will be deducted.                                                 */
/******************************************************************************/

/*
* Author:      Bizuwork Melesse
* 
*/

#ifndef _WARMUP1_H
#define _WARMUP1_H

#include "cs402.h"

typedef enum {
	FileOpen,
	LongLine,
	MalformedLine,
	Duplicate,
	HighAmount,
	HighBalance,
	UnknownFlag,
	UnknownAmount,
	InvalidTimestamp,
	Malloc,
	ListInsertion,
	TooManyArgs,
	UnknownCmd,
	TooFewArgs

} ErrorType;

typedef enum {

	DEPOSIT,
	WITHDRAWAL
} Flag;

typedef struct TransactionRecord {

	char*date;
	char*desc;
	int amount;
	int balance;
	int dateRaw;
	Flag flag;

} Transaction;



#endif /*_WARMUP1_H*/
/*
*/
extern int readInput(My402List*, char*, FILE*);
extern Flag getFlag(char*);
extern int getAmount(char*);
extern int getRawDate(char*);
extern void exitOnError(ErrorType);
extern Transaction getTransaction(char*, char*, char*, char*);
extern int validateFormat(char*, char*, char*, char*);
extern Transaction parseLine(char*);
extern Transaction* copyTransaction(Transaction record);
extern void insertTransaction(My402List*, Transaction);
extern void printHistory(My402List*);


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
extern void validateLine(char* );

extern void processArgs(int, char* [], char**, FILE**);

extern char* cleanString(char*);