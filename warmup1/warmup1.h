/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#ifndef _WARMUP1_H
#define _WARMUP1_H

#include "cs402.h"
#include "my402list.h"
#include "error.h"
#include "transaction.h"

int lineNum;



/*
	Read input data either from a file or stdin one line at a time. 
*/
extern int readInput(My402List*, char*, FILE*);


/*
	Parse an input line by tokenizing it. Return a new Transaction record
	with the tokenized information.
*/
extern Transaction parseLine(char*);


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
	Perform regex matching for a particular transaction attribute. 
	If matching fails, throw the corresponding error message.
*/
extern void matchPattern(char*, char*, ErrorType);

#endif /*_WARMUP1_H*/

