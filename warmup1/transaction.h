/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#ifndef _WARMUP1_TRANSACTION
#define _WARMUP1_TRANSACTION

#include "my402list.h"

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
	Perform the appropriate type casting and formatting for each 
	transaction attribute and return a Transaction record.
*/
extern Transaction getTransaction(char*, char*, char*, char*);

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
	Compute the balance after the current transaction takes place
	for every record of transaction. 
*/
extern void computeBalance(My402List*);

#endif