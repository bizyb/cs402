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
*  Author:      Bizuwork Melesse
* 
*  
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <sys/time.h>
#include <time.h> // TODO: remove this and uncomment sys/time.h

#include "cs402.h"
#include "my402list.h"
#include "warmup1.h"
#include <regex.h>



// static char gszProgName[MAXPATHLENGTH];
const int BUFFERSIZE = 1028;
const int MAX_LINE = 1024;
const double FACTOR = 100.00;
const int MAX_AMOUNT = 1000000000; // $10,000,000.00
const int MAX_DATE_LENGTH = 10;
const int BASE = 10;


/* ----------------------- Utility Functions -----------------------*/
void exitOnError(ErrorType e) {

    // TODO: do we have to handle line length?
    char* msg;
    switch(e) {
        case FileOpen: msg = "Failed to open file."; break;
        case LongLine: msg = "Line exceeds acceptable length."; break;
        case MalformedLine: msg = "Invalid line formatting."; break;
        case Duplicate: msg = "Found entries with duplicate timestamp."; break;
        case HighAmount: msg = "Amount exceeds allowed limit."; break;
        case HighBalance: msg = "Balance exceeds allowed limit."; break;
        case UnknownFlag: msg = "Unknown transaction flag."; break;
        case UnknownAmount: msg = "Unable to parse amount."; break;
        case InvalidTimestamp: msg = "Invalid timestamp."; break;
        case Malloc: msg = "Failed to allocate memory"; break;
        case ListInsertion: msg = "Failed to add transaction record into list."; break;
        case TooManyArgs: msg = "Too many command line arguments."; break;
        case TooFewArgs: msg = "Not enough command line arguments."; break;
        case UnknownCmd: msg = "Unknown command. Did you mean to say 'sort'?"; break;
        default: msg = "Unknown error";
    }

    fprintf(stderr, "%s Exting program...\n", msg);
    exit(1);

}

void freeMemory(My402List* pList, My402ListElem* elem) {
    
    Transaction* recPtr = (Transaction*) elem->obj;
    free(recPtr->date);
    free(recPtr->desc);
    free(recPtr);
    My402ListUnlink(pList, elem);
    


}

void printHistory(My402List*  pList) {

    
    // TODO: add ???.. for amount/balance exceeding 10mil
    // TODO: date formatting

    My402ListElem* curr = pList->anchor.next;
    My402ListElem* prev = NULL;
    char* border = "+-----------------+--------------------------+----------------+----------------+";
    fprintf(stdout, "%s\n", border);
    fprintf(stdout, "|       Date      ");
    fprintf(stdout, "| Description              ");
    fprintf(stdout, "|         Amount ");
    fprintf(stdout, "|        Balance |\n");
    fprintf(stdout, "%s\n", border);


    while (curr != NULL) {
        Transaction record = *(Transaction* ) curr->obj;

        fprintf(stdout, "| %s ", cleanString(record.date));
        fprintf(stdout, "| %s", cleanString(record.desc));

        char* amount = cleanString(formatCurrency(record.amount));
        if (record.flag == WITHDRAWAL) fprintf(stdout, "| (%s) ", amount);
        else fprintf(stdout, "|  %s  ", amount);
        
        char* balance = cleanString(formatCurrency(record.balance));
        if (record.flag == WITHDRAWAL) fprintf(stdout, "| (%s) |\n", balance);
        else fprintf(stdout, "|  %s  |\n", balance);

        prev = curr;
        curr = My402ListNext(pList, curr);

        freeMemory(pList, prev);
        // My402ListUnlink(pList, prev);
        free(amount);
        free(balance);
    }
    fprintf(stdout, "%s\n", border);
    My402ListUnlinkAll(pList);

}

char* cleanString(char* dirtyString) {

    unsigned int i;
    for (i = 0; i < strlen(dirtyString); i++) {

        if (dirtyString[i] == '\n') dirtyString[i] = ' ';

    }
    return dirtyString;
}

Flag getFlag(char* flag) {

    Flag tFlag;

    if (*flag == '-') tFlag = WITHDRAWAL;
    else if (*flag == '+') tFlag = DEPOSIT;
    else {
        ErrorType e = UnknownFlag;
        exitOnError(e);
    }
    return tFlag;
}
int getAmount(char* amount) {

    char* end;
    double dAmount = strtod(amount, &end);
    if (amount == end) {
        // if conversion fails, address of amount gets stored at address of end
        // fprintf(stdout, "failed on amount: %s\n", amount);
        ErrorType e = UnknownAmount;
        exitOnError(e);
    }

    int iAmount = round(dAmount * FACTOR);
    if (iAmount >= MAX_AMOUNT) {

        ErrorType e = HighAmount;
        exitOnError(e);
    }
    return iAmount;

}

int getRawDate(char* date) {

    unsigned int length = strlen(date);
    char* end;
    int iDate = strtol(date, &end, BASE);
    if (length > MAX_DATE_LENGTH || date == end) {

        ErrorType e = InvalidTimestamp;
        exitOnError(e);
    }

    return iDate;
}

char* getDesc(char* desc) {

    unsigned int length = strlen(desc);
    const unsigned int DESC_LENGTH = 23;
    const unsigned int CELL_LENGTH = 26;

    // TODO: This will lead to some leakage so need to do something about it
    char * abridged = (char *) calloc(CELL_LENGTH +1, sizeof(char)); 

    unsigned int i;

    if (length < DESC_LENGTH) {
        
        for (i = 0; i < length; i++) {
            abridged[i] = desc[i];
        }
        for (i = length; i < CELL_LENGTH; i++) {
            abridged[i] = ' '; //pad it with spaces
        }        
    }
    else {
         for (i = 0; i < DESC_LENGTH; i++) {
            abridged[i] = desc[i];
        }
        for (i = DESC_LENGTH; i < CELL_LENGTH ; i++) {
            abridged[i] = ' '; //pad it with spaces
        }
    }
    abridged[CELL_LENGTH-1] = '\0';
    return abridged;
}

char* formatDate(int rawDate) {

    const int DATE_LENGTH = 16;
    char* delim = " ";
    char* day, *month, *date, *year;
    char* buffer = (char* ) calloc(DATE_LENGTH +1, sizeof(char)); //TODO: malloc needs to be freed
    time_t t = rawDate;
    char* fTime = ctime(&t);

    
    day = strtok(fTime, delim);
    month = strtok(NULL, delim);
    date = strtok(NULL, delim);
    (void)strtok(NULL, delim); // ignore the hour
    year = strtok(NULL, delim);


    sprintf(buffer, "%s %s %s %s", day, month, date, year);
    buffer[DATE_LENGTH-1] = '\0';
   
    return buffer;

}

char* formatCurrency(int value) {

    const int FIELD_WIDTH = 12;
   
   //TODO: free up the memory in printHistory
    char* buffer = (char*) calloc(FIELD_WIDTH + 1, sizeof(char));
    double fAmount = (value / FACTOR);
    sprintf(buffer, "%.2f", fAmount);
    int newLength = insertComma(buffer);

    int i, j = 0;
    char c = ' ';
    // right-justify
    for (i = FIELD_WIDTH - 1; i >= 0; i--) {

        if (j >= newLength) buffer[i] = c;
        else {
            buffer[i] = buffer[newLength - j - 1];
            j++;
        }
    }
    
    return buffer;

}

int insertComma(char* buffer) {

    const int FIELD_WIDTH = 12;
    const int FIRST_COMMA_POS = 6;
    const int SECOND_COMMA_POS = 10;
    int length;

    for (length = 0; length < FIELD_WIDTH; length++) {

        if (buffer[length] == '\0') break;
    }

    char *temp = (char*) calloc(FIELD_WIDTH + 1, sizeof(char));
    char c = ',';
    int n;
    if (length > FIRST_COMMA_POS) {

        n = length - FIRST_COMMA_POS;
        strncpy(temp, buffer, n);
        temp[n] = c;
        strncpy(&temp[n+1], &buffer[n], FIRST_COMMA_POS);
        strncpy(buffer, temp, FIELD_WIDTH);
        length++;
    }
  
    if (length  > SECOND_COMMA_POS) {
        
        n = length - SECOND_COMMA_POS;
        strncpy(temp, buffer, n);
        temp[n] = c;
        strncpy(&temp[n+1], &buffer[n], SECOND_COMMA_POS);
        strncpy(buffer, temp, FIELD_WIDTH);
        length++;

    }
    free(temp);
    return length;


}
Transaction getTransaction(char* flag, char* date, char* amount, char* desc) {

    char* tDate,* tDesc;
    int tAmount, tBalance, tRawDate;
    Flag tFlag;

    tFlag = getFlag(flag);
    tRawDate = getRawDate(date);
    tDate = formatDate(tRawDate);
    tDesc = getDesc(desc);
   
    tAmount = getAmount(amount);


    tBalance = 0;

    Transaction record = {tDate, tDesc, tAmount, tBalance, tRawDate, tFlag};
    return record;
}

Transaction* copyTransaction(Transaction record, Transaction* recPtr) {

    // TODO: check all malloc and memcpy return values
    
    recPtr->date = (char* ) calloc(strlen(record.date) + 1, sizeof(char));
    recPtr->desc = (char* ) calloc(strlen(record.desc) + 1, sizeof(char));
   
    (void)memcpy((void* ) recPtr->date, (void* ) record.date, (strlen(record.date)+1)*sizeof(char));
    (void)memcpy((void* ) recPtr->desc, (void* ) record.desc, (strlen(record.desc)+1)*sizeof(char));
    (void)memcpy((void* ) &recPtr->amount, (void* ) &record.amount, sizeof(int));
    (void)memcpy((void* ) &recPtr->balance, (void* ) &record.balance, sizeof(int));
    (void)memcpy((void* ) &recPtr->dateRaw, (void* ) &record.dateRaw, sizeof(int));
    (void)memcpy((void* ) &recPtr->flag, (void* ) &record.flag, sizeof(Flag));
    
    free(record.date);
    free(record.desc);
    // if (recPtr == NULL) {
    //     ErrorType e = Malloc;
    //     exitOnError(e);
    // }
    return recPtr;

}

void insertTransaction(My402List* pList, Transaction record) {

    //iterate through the list and insert the record in a sorted order
    // if another record with the same timestamp exists, call exitOnError to exit the program
   
    Transaction* recPtr = (Transaction* ) calloc(1, sizeof(Transaction));
    int result = My402ListAppend(pList, (void* ) copyTransaction(record, recPtr));
    if (result == FALSE) {

        ErrorType e = ListInsertion;
        exitOnError(e);
    }

    
}

void validateLine(char* line) {

    //match formatting
    regex_t patternBuffer;
    char* pattern = "(\\+|\\-)(\t([0-9]{1,10})\t([0-9]{1,7}\\.[0-9]{2})\t\\w+)";
    int formatFail = regcomp(&patternBuffer, pattern, REG_EXTENDED);
    formatFail = regexec(&patternBuffer, line, 0, NULL, 0);
    regfree(&patternBuffer);

    //verify line width
    int i;
    for(i = 0; i < BUFFERSIZE; i++) {

        if (line[i] == '\n') break;
    }

    if (formatFail) {

        ErrorType e = MalformedLine;
        exitOnError(e);
    }
    if (i + 1 > MAX_LINE) {

        ErrorType e = LongLine;
        exitOnError(e);
    }

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

    if (fileName != NULL) {

        file = fopen(fileName, "r");
        if (file == NULL) {
            ErrorType e = FileOpen;
            exitOnError(e);
        }
    }
    else file = inStream;

    
    while (fgets(buffer, BUFFERSIZE,  file) != NULL) {
        if (buffer[0] != '\n') {
            Transaction record = parseLine((char* ) &buffer);
            insertTransaction(pList, record);
        }
        
    }
    fclose(file);
    return TRUE;

}

void processArgs(int argc, char *argv[], char** fileNamePtr, FILE** inStreamPtr) {

    const int MAX_ARG_COUNT = 3;

    char* command = "sort";
    if (argc > MAX_ARG_COUNT) exitOnError(TooManyArgs);
    else if (argc == MAX_ARG_COUNT) {

        if (strcmp(command, argv[1]) != 0) exitOnError(UnknownCmd);
        *fileNamePtr = argv[MAX_ARG_COUNT-1];
     
    }
    else if (argc == MAX_ARG_COUNT - 1) {

        if (strcmp(command, argv[1]) != 0) exitOnError(UnknownCmd);
        *inStreamPtr = stdin;
    }

    else exitOnError(TooFewArgs); 

    
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
    if (list.num_members) printHistory(&list);
    // freeMemory(&list);
    
    return(0);
}
