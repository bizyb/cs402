/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "cs402.h"
#include "my402list.h"
#include "warmup1.h"
#include "error.h"
#include "transaction.h"
#include "global.h"
#include "history.h"


/* ----------------------- Utility Functions -----------------------*/



void matchPattern(char* line, char* pattern, ErrorType e) {

    regex_t patternBuffer;
    int formatFail;

    formatFail  = regcomp(&patternBuffer, pattern, REG_EXTENDED);
    formatFail = regexec(&patternBuffer, line, 0, NULL, 0);
    regfree(&patternBuffer);

    if (formatFail) exitOnError(e);



}
void validateLine(char* line) {

    //verify formatting
    // Note that the pattens are additive. If the previous patten fails, the next
    // one never gets executed since the program exits. This helps us avoid any 
    // ambiguities when it comes to matching the description, which can contain
    // character any number of times. 

    char* flagPattern = "^(\\+|\\-)\t";
    char* datePatten =  "^(\\+|\\-)(\t[0-9]{1,10}\t)";
    char* currencyPattern = "^(\\+|\\-)(\t[0-9]{1,10}\t([0-9]{1,7}\\.[0-9]{2})\t)";
    char* descPattern = "^(\\+|\\-)(\t[0-9]{1,10}\t([0-9]{1,7}\\.[0-9]{2})\t)[A-Za-z0-9_]+";
    // char* linePattern = "(\\+|\\-)(\t([0-9]{1,10})\t([0-9]{1,7}\\.[0-9]{2})\t[A-Za-z0-9_]+)";

   
    matchPattern(line, flagPattern, FlagFormat);
    matchPattern(line, datePatten, DateFormat);
    matchPattern(line, currencyPattern, CurrencyFormat);
    matchPattern(line, descPattern, DescFormat);

    //verify line width
    int i;
    for(i = 0; i < BUFFERSIZE; i++) {

        if (line[i] == '\n') break;
    }

    // if (formatFail) exitOnError(MalformedLine);
    if (i + 1 > MAX_LINE) exitOnError(LongLine);

    // Redundant: regex matching would not allow this
    if (i + 1 < MIN_LINE) exitOnError(LineTooShort);

}

Transaction parseLine(char* line) {

    char* delim = "\t";
    char* flag,* date,* amount,* desc;

    validateLine(line);
    
    flag = strtok(line, delim);
    date = strtok(NULL, delim);
    amount = strtok(NULL, delim);
    desc = strtok(NULL, delim);

    if (desc[strlen(desc)-1] == '\n')  desc[strlen(desc)-1] = '\0';

    return getTransaction(flag, date, amount, desc);
}

int readInput(My402List* pList, char* fileName, FILE* inStream) {

    char buffer [BUFFERSIZE];
    FILE* file;
    lineNum = 1;

    if (fileName != NULL) {

        file = fopen(fileName, "r");
        if (file == NULL)  exitOnError(FileOpen);

    }

    else file = inStream;

    while (fgets(buffer, BUFFERSIZE,  file) != NULL) {
        if (buffer[0] != '\n') {
            Transaction record = parseLine((char* ) &buffer);
            insertTransaction(pList, record);
            lineNum++;
        }
        else exitOnError(EmptyLine);
        
    }
    fclose(file);
    return TRUE;

}

void processArgs(int argc, char *argv[], char** fileNamePtr, FILE** inStreamPtr) {

    
    const int MAX_ARG_COUNT = 3;

    char* command = "sort";
    if (argc > MAX_ARG_COUNT) exitOnCmdError(TooManyArgs);
    else if (argc == MAX_ARG_COUNT) {

        if (strcmp(command, argv[1]) != 0) exitOnCmdError(UnknownCmd);
        *fileNamePtr = argv[MAX_ARG_COUNT-1];
        exitOnFileError(*fileNamePtr); 

     
    }
    else if (argc == MAX_ARG_COUNT - 1) {

        if (strcmp(command, argv[1]) != 0) exitOnCmdError(UnknownCmd);
        *inStreamPtr = stdin;
    }

    else exitOnCmdError(TooFewArgs); 

    
}



/* ----------------------- main() -----------------------*/

int main(int argc, char* argv[])
{
    char* fileName = NULL;
    FILE* inStream = NULL;
    My402List list;

    processArgs(argc, argv, &fileName, &inStream);

    memset(&list, 0, sizeof(My402List));
    (void)My402ListInit(&list);

    readInput(&list, fileName, inStream);
    if (list.num_members) {

        computeBalance(&list);
        printHistory(&list);

    } 
  
    return(0);
}

