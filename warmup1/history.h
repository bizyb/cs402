
#ifndef _WARMUP1_HISTORY
#define _WARMUP1_HISTORY

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

#endif