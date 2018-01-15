/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 programming assignments at USC.              */
/*         53616c7465645f5f2e8d450c0c5851acd538befe33744efca0f1c4f9fb5f       */
/*         3c8feabc561a99e53d4d21951738da923cd1c7bbd11b30a1afb11172f80b       */
/*         984b1acfbbf8fae6ea57e0583d2610a618379293cb1de8e1e9d07e6287e8       */
/*         de7e82f3d48866aa2009b599e92c852f7dbf7a6e573f1c7228ca34b9f368       */
/*         faaef0c0fcf294cb                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 * Author:      Bizuwork Melesse
 *
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"
#include "my402list.h"
#include "warmup1.h"

// static char gszProgName[MAXPATHLENGTH];
const int BUFFERSIZE = 81;
const double FACTOR = 100.0;
const int MAX_AMOUNT = 1000000000; // $10,000,000.00
const int MAX_DATE_LENGTH = 10;


int gnDebug=0;
int gnSeed=0;


/* ----------------------- Utility Functions ----------------------- */
void printError(ErrorType e) {

    // TODO: do we have to handle line length?
    char *msg;
    switch(e) {
        case FileOpen: msg = "Failed to open file."; break;
        case LongLine: msg = "Line exceeds acceptable length."; break;
        case MalformedLine: msg = "Unacceptable line formatting."; break;
        case Duplicate: msg = "Found entries with duplicate timestamp."; break;
        case HighAmount: msg = "Amount exceeds allowed limit."; break;
        case HighBalance: msg = "Balance exceeds allowed limit."; break;
        case UnknownFlag: msg = "Unknown transaction flag."; break;
        case UnknownAmount: msg = "Unable to parse amount."; break;
        case InvalidTimestamp: msg = "Invalid timestamp."; break;
        default: msg = "Unknown error";
    }

    // if (e == FileOpen) msg = "Failed to open file.";
    // else if (e == LongLine) msg = "Line exceeds acceptable length.";
    // else if (e == MalformedLine) msg = "Unacceptable line formatting.";
    // else if (e == Duplicate) msg = "Found entries with duplicate timestamp.";
    // else if (e == HighAmount) msg = "Amount exceeds allowed limit.";
    // else if (e == HighBalance) msg = "Balance exceeds allowed limit.";
    // else if (e == UnknownFlag) msg = "Unknown transaction flag.";
    // else if (e == UnknownAmount) msg = "Unable to parse amount.";
    // else if (e == InvalidTimestamp) msg = "Invalid timestamp.";
    fprintf(stderr, "%s Exting program...\n", msg);
    // exit()

}

void printHistory(Transaction record) {

    fprintf(stdout, "%s ", record.date);
    fprintf(stdout, "%s ", record.desc);
    fprintf(stdout, "%d ", record.amount);
    fprintf(stdout, "%d\n", record.balance);

}

Flag getFlag(char *flag) {

    Flag tFlag;

    if (*flag == '-') tFlag = WITHDRAWAL;
    else if (*flag == '+') tFlag = DEPOSIT;
    else {
        ErrorType e = UnknownFlag;
        printError(e);
    }
    return tFlag;
}
int getAmount(char *amount) {

    char *end;
    double dAmount = strtod(amount, &end);
    if (amount == end) {
        // if conversion fails, address of amount gets stored at address of end
        ErrorType e = UnknownAmount;
        printError(e);
    }

    int iAmount = (int) (dAmount * FACTOR);
    if (iAmount >= MAX_AMOUNT) {

        ErrorType e = HighAmount;
        printError(e);
    }
    return iAmount;

}

int getRawDate(char *date) {

    unsigned int length = strlen(date);
    fprintf(stdout, "length of date: %d\n", length);
    if (length > MAX_DATE_LENGTH) {

        ErrorType e = InvalidTimestamp;
        printError(e);
    }

    return (int) *date;
}
Transaction getTransaction(char *flag, char *date, char *amount, char *desc) {

    char *tDate, *tDesc;
    int tAmount, tBalance, tRawDate;
    Flag tFlag;

    tFlag = getFlag(flag);
    tRawDate = getRawDate(date);
    tDate = date; //formatDate(), no need to do error handling since getDate() succeeded
    tDesc = desc; //getDesc(), check that it's not empty
   
    tAmount = getAmount(amount);

    tBalance = 0; //for now; will be computed after sorting; when iterating through the sorted list, 
    //keep local max abs(balance). Handle >=10mil accordingly 



    Transaction record = {tDate, tDesc, tAmount, tBalance, tRawDate, tFlag};
    return record;
}

void insertRecord(Transaction record) {

    //iterate through the list and insert the record in a sorted order
    // if another record with the same timestamp exists, call printError to exit program

}
int validateFormat(char *flag, char *date, char *amount, char *desc) {

    return TRUE;
}
Transaction parseLine(char *line) {

    char *delim = "\t";
    char *flag, *date, *amount, *desc;
    flag = strtok(line, delim);
    date = strtok(NULL, delim);
    amount = strtok(NULL, delim);
    desc = strtok(NULL, delim);

    if (!validateFormat(flag, date, amount, desc)) {
        ErrorType e = MalformedLine;
        printError(e);
    }

    return getTransaction(flag, date, amount, desc);
    




    // fprintf(stdout, "%s %s %s %s\n", flag, date, amount, desc);

    // while (tokens != NULL) {
    //      fprintf(stdout, "%s\n", tokens);
    //      tokens = strtok(NULL, &delim);
    // }
    // unsigned int length = strlen(tokens);
   
    // fprintf(stdout, "%d\n", length);

    // return TRUE;
}
int readInput(char *path) {
    char buffer [BUFFERSIZE];
    FILE *file;

    file = fopen(path, "r");
    if (file == NULL) {
        ErrorType e = FileOpen;
        printError(e);
    }
    while (fgets(buffer, BUFFERSIZE,  file) != NULL) {
        Transaction record = parseLine((char *) &buffer);
        // insertRecord(record);
        printHistory(record);
        // fprintf(stdout, "%s\n", buffer);
        // fprintf(stdout, "length: %d\n", length);
    }

    return TRUE;

}

/* ----------------------- main() ----------------------- */

int main(int argc, char *argv[])
{
    char *path = "tfile.txt";
    readInput(path);
    
    return(0);
}
