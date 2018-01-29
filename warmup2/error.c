/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 2
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>


#include "error.h"
#include "parent.h"
#include "cs402.h"

void exitOnError(ErrorType e) {
 
    char* msg;
    switch(e) {
        
        case LongLine:      msg = "Line exceeds acceptable length."; break;
        case NumPackets:    msg = "Invalid Number of Packets."; break;
        case InterArrival:  msg = "Invalid inter-arrival time."; break;
        case Tokens:        msg = "Invalid number of tokens."; break;
        case ServiceTime:   msg = "Invalid service time."; break;
        // case Timestamp:     msg = "Invalid timestamp."; break;
        case Malloc:        msg = "Failed to allocate memory"; break;
        case ListInsertion: msg = "Failed to enqueue packet."; break;
        case EmptyLine:     msg = "Empty line."; break;
        // case LineTooShort:  msg = "Not enough characters."; break;
        
        default: msg = "Unknown error";
    }

    if (e == Malloc || e == ListInsertion) {

        fprintf(stderr, "%s\n", msg);
    }
    else {

        fprintf(stderr, "Error on line %d: %s\n", lineNum, msg);

    }
    
    exit(1);

}


void exitOnCmdError(ErrorType e) {

    char* usage = "Usage: ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]";
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
    else {

        FILE* f = fopen(fileName, "r");
        if (errno == EACCES) {

            sprintf((char*)&msg, "Input file \'%s\' cannot be opened - access denied", fileName);
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
        if (f != NULL) fclose(f);
    }


    if (error) {

       
        fprintf(stderr, "(%s)\n", msg);
        exit(1);
    }

}
