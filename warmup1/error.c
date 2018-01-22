/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


#include "error.h"
#include "warmup1.h"
#include "cs402.h"

void exitOnError(ErrorType e) {
 
    char* msg;
    switch(e) {
        case FileOpen:      msg = "Failed to open file."; break;
        case LongLine:      msg = "Line exceeds acceptable length."; break;
        case Duplicate:     msg = "Found entries with duplicate timestamp."; break;
        case HighAmount:    msg = "Amount exceeds allowed limit."; break;
        case HighBalance:   msg = "Balance exceeds allowed limit."; break;
        case LowBalance:    msg = "Balance is below allowed limit."; break;
        case UnknownFlag:   msg = "Unknown transaction flag."; break;
        case UnknownAmount: msg = "Unable to parse amount."; break;
        case Timestamp:     msg = "Invalid timestamp."; break;
        case Malloc:        msg = "Failed to allocate memory"; break;
        case ListInsertion: msg = "Failed to add transaction record into list."; break;
        case EmptyLine:     msg = "Empty line."; break;
        case LineTooShort:  msg = "Not enough characters."; break;
        case FlagFormat:    msg = "Invalid transaction flag."; break;
        case DateFormat:    msg = "Invalid date."; break;
        case CurrencyFormat:msg = "Invalid currency."; break;
        case DescFormat:    msg = "Invalid/missing description."; break;
        default: msg = "Unknown error";
    }

    if (e == Malloc || e == ListInsertion || e == FileOpen || e == LowBalance
        || e == HighBalance) {

        fprintf(stderr, "%s\n", msg);
    }
    else {

        fprintf(stderr, "Error on line %d: %s\n", lineNum, msg);

    }
    
    exit(1);

}


void exitOnCmdError(ErrorType e) {

    char* usage = "Usage: ./warmup1 sort [tfile]";
    char* msg;

    switch(e) {

        case TooFewArgs:    msg = "Not enough arguments."; break;
        case TooManyArgs:   msg = "Too many arguments."; break;
        case UnknownCmd:    msg = "Command not recognized."; break;
        default: msg = "Unknown error";
    }

    fprintf(stderr, "Malformed Command: %s\n%s\n", msg, usage);
    exit(1);
}

void exitOnFileError(char* fileName) {

    int error = FALSE;
    const int ERROR_BUFFER_SIZE = 100;
    struct stat status;

    char msg[ERROR_BUFFER_SIZE];
    (void)memset(&msg, 0, (ERROR_BUFFER_SIZE)*sizeof(char));

    
   stat(fileName, &status);

    if (S_ISDIR(status.st_mode)) {

        sprintf((char*)&msg, "Input file \'%s\' is a directory", fileName);
        error = TRUE;
    }

    else if (access(fileName, F_OK) != 0) {

        sprintf((char*)&msg, "No such file or directory named \'%s\'", fileName);
        error = TRUE;

    }

    else if (access(fileName, R_OK) != 0) {

        sprintf((char*)&msg, "Input file \'%s\' cannot be opened - access denied", fileName);
        error = TRUE;

    }


    if (error) {

        fprintf(stderr, "(%s)\n", msg);
        exit(1);
    }

}
