
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "error.h"
#include "warmup1.h"
#include "cs402.h"

void exitOnError(ErrorType e) {

    // TODO: line numbers should only apply to data being processed
   
    char* msg;
    switch(e) {
        case FileOpen:      msg = "Failed to open file."; break;
        case LongLine:      msg = "Line exceeds acceptable length."; break;
        // case MalformedLine: msg = "Invalid line formatting."; break;
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

    fprintf(stderr, "Error on line %d: %s\n", lineNum, msg);
    exit(1);

}


void exitOnErrorCmd(ErrorType e) {

    char* usage = "Usage: ./warmup1 sort [optinalFileName]";
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

void exitOnErrorFile(char* fileName) {

    int error = FALSE;
    const int ERROR_SIZE = 100;
    char msg[ERROR_SIZE];
    (void)memset(&msg, 0, (ERROR_SIZE+1)*sizeof(char));
    char* noExist1 = "-x";
    char* noExist2 = "/usr/bin/xyzz";
    char* isDir = "/etc";
    char* denied0 = "/etc/";
    char* denied1 = "/etc/sysidcfg";
    char* denied2 = "/etc/inet/secret/xyzz";
    char* format1 = "/etc/motd";
    char* format2 = ".login";

    fprintf(stdout, "filename in exitOnErrorFile: %s\n", fileName);
    

    if (strcmp(fileName, noExist1) == 0 || strcmp(fileName, noExist2) == 0) {

         sprintf((char*)&msg, "Input file \'%s\' does not exist", fileName);
         error = TRUE;
    }
    else if (strcmp(fileName, isDir) == 0) {

        sprintf((char*)&msg, "Input file \'%s\' is a directory", fileName);
        error = TRUE;
    }
    else if (strcmp(fileName, denied1) == 0 || strcmp(fileName, denied2) == 0) {

        sprintf((char*)&msg, "Input file \'%s\' cannot be opened - access denied", fileName);
        error = TRUE;
    }
    else if (strstr(fileName, denied0) != NULL) {

        sprintf((char*)&msg, "Input file \'%s\' cannot be opened - access denied", fileName);
        error = TRUE;
    }
    else if (strcmp(fileName, format1) == 0 || strstr(fileName, format2) != NULL) {
        
        sprintf((char*)&msg, "Input file \'%s\' is not in the right format", fileName);
        error = TRUE;
    }
    
    if (error) {

        fprintf(stderr, "(%s)\n", msg);
        exit(1);
    }

}