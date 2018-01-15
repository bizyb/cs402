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
int BUFFERSIZE = 81;


int gnDebug=0;
int gnSeed=0;


/* ----------------------- Utility Functions ----------------------- */
void printError(ErrorType e) {

    char *msg;
    if (e == FileOpen) msg = "Failed to open file.";
    else if (e == LongLine) msg = "Line exceeds acceptable length.";
    else if (e == MalformedLine) msg = "Unacceptable line formatting.";
    else if (e == Duplicate) msg = "Found entries with duplicate timestamp.";
    fprintf(stderr, "%s Exting program...\n", msg);
    // exit()

}
int validateFormat(char *flag, char *date, char *amount, char *desc) {


}
int parseLine(char *line) {

    char delim = '\t';
    char *flag, *date, *amount, *desc;
    flag = strtok(line, &delim);
    date = strtok(NULL, &delim);
    amount = strtok(NULL, &delim);
    desc = strtok(NULL, &delim);

    if (!validateFormat(flag, date, amount, desc)) {
        ErrorType e = MalformedLine;
        printError(e);
    }

    
    fprintf(stdout, "%s %s %s %s\n", flag, date, amount, desc);

    // while (tokens != NULL) {
    //      fprintf(stdout, "%s\n", tokens);
    //      tokens = strtok(NULL, &delim);
    // }
    // unsigned int length = strlen(tokens);
   
    // fprintf(stdout, "%d\n", length);

    return TRUE;
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
        (void)parseLine((char *) &buffer);
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
