/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <sys/time.h>

#include <time.h>
#include "cs402.h"
#include "my402list.h"
#include "warmup1.h"
#include <regex.h>
// #include <errno.h>
// #include <string.h>

#include "error.h"



const int BUFFERSIZE = 1028;
const int MAX_LINE = 1024;
const int MIN_LINE = 10;
const double FACTOR = 100.00;
const int MAX_AMOUNT = 1000000000; // $10,000,000.00
const int MAX_DATE_LENGTH = 10;
const int BASE = 10;


// error.h:
//     exitOnError
//     exitOnErrorCmd
//     exitOnErrorFile

// transaction.h:
//     all get* functions related to transaction attributes
//     copyTransaction
//     insertTransaction
//     computeBalance

// data.h:
//     arg processing 
//     read file
//     validate line
//     parse line

// history.h
//     printHistory
//     format* functions 
//     cleanString
//     free memory
    



/* ----------------------- Utility Functions -----------------------*/


void freeMemory(My402List* pList, My402ListElem* elem) {
    
    Transaction* recPtr = (Transaction*) elem->obj;
    free(recPtr->date);
    free(recPtr->desc);
    free(recPtr);
    My402ListUnlink(pList, elem);

}

void printHistory(My402List*  pList) {

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
        if (record.balFlag == NEGATIVE) fprintf(stdout, "| (%s) |\n", balance);
        else fprintf(stdout, "|  %s  |\n", balance);

        prev = curr;
        curr = My402ListNext(pList, curr);

        freeMemory(pList, prev);
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
    else  exitOnError(UnknownFlag);

    return tFlag;
}
int getAmount(char* amount) {

    char* end;
    double dAmount = strtod(amount, &end);

    // if conversion fails, address of amount gets stored at address of end
    if (amount == end)  exitOnError(UnknownAmount);

    int iAmount = round(dAmount * FACTOR);
    if (iAmount >= MAX_AMOUNT) exitOnError(HighAmount);

    return iAmount;

}

int getRawDate(char* date) {

    unsigned int length = strlen(date);
    char* end;
    int iDate = strtol(date, &end, BASE);

    if (length > MAX_DATE_LENGTH || date == end) exitOnError(Timestamp);

    return iDate;
}

char* getDesc(char* desc) {

    unsigned int length = strlen(desc);
    const unsigned int DESC_LENGTH = 23;
    const unsigned int CELL_LENGTH = 26;

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
    char* buffer = (char* ) calloc(DATE_LENGTH +1, sizeof(char)); 
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
   
    char* buffer = (char*) calloc(FIELD_WIDTH + 1, sizeof(char));
    double fAmount = (value / FACTOR);

    // negate any negative value (we still retain the sign of the original 
    // value through the one of the currency flags)
    if (fAmount < 0) fAmount *= -1;
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

    if (recPtr->date == NULL || recPtr->desc == NULL) exitOnError(Malloc);
   
    (void)memcpy((void* ) recPtr->date, (void* ) record.date, (strlen(record.date)+1)*sizeof(char));
    (void)memcpy((void* ) recPtr->desc, (void* ) record.desc, (strlen(record.desc)+1)*sizeof(char));
    (void)memcpy((void* ) &recPtr->amount, (void* ) &record.amount, sizeof(int));
    (void)memcpy((void* ) &recPtr->balance, (void* ) &record.balance, sizeof(int));
    (void)memcpy((void* ) &recPtr->dateRaw, (void* ) &record.dateRaw, sizeof(int));
    (void)memcpy((void* ) &recPtr->flag, (void* ) &record.flag, sizeof(Flag));
    
    free(record.date);
    free(record.desc);
    return recPtr;

}

void insertTransaction(My402List* pList, Transaction record) {

   
    My402ListElem *elem = NULL;
    int inserted = FALSE;

    Transaction* recPtr = (Transaction* ) calloc(1, sizeof(Transaction));
    recPtr =  copyTransaction(record, recPtr);

    for(elem = My402ListFirst(pList); elem != NULL; elem=My402ListNext(pList, elem)) {

        Transaction* currRecordPtr = (Transaction*) elem->obj;

        if (currRecordPtr->dateRaw == recPtr->dateRaw) {
            // fprintf(stdout, "currRecordPtr->dateRaw: %d\n", currRecordPtr->dateRaw );
            // fprintf(stdout, "recPtr->dateRaw: %d\n", recPtr->dateRaw);
            exitOnError(Duplicate);
        }
        else if (recPtr->dateRaw < currRecordPtr->dateRaw) {

            inserted = My402ListInsertBefore(pList, (void*)recPtr, elem);
            if (!inserted) exitOnError(ListInsertion);
            break;

        }

    }
    if (!inserted) My402ListAppend(pList, (void*)recPtr);
    
    
}
void computeBalance(My402List* pList) {

    My402ListElem *elem = NULL;
    int balance;
    for(elem = My402ListFirst(pList); elem != NULL; elem=My402ListNext(pList, elem)) {

        Transaction* currRecordPtr = (Transaction*) elem->obj;
        Transaction* prevRecordPtr;
        if (elem->prev == &pList->anchor) {

            balance = currRecordPtr->amount;
            currRecordPtr->balance = currRecordPtr->flag == DEPOSIT ? balance : -1 * balance;
            currRecordPtr->balFlag = currRecordPtr->flag == DEPOSIT ? POSITIVE : NEGATIVE;

        }
        else {

            prevRecordPtr = (Transaction*) elem->prev->obj;
            balance = prevRecordPtr->balance;

            if (currRecordPtr->flag == WITHDRAWAL) balance -= currRecordPtr->amount;
            else balance += currRecordPtr->amount;

            if (balance < 0) currRecordPtr->balFlag = NEGATIVE; 
            else currRecordPtr->balFlag = POSITIVE;

            currRecordPtr->balance = balance;
            // fprintf(stdout, "balance: %d\n", balance);    
        }
     
        if ( currRecordPtr->balance >= MAX_AMOUNT) exitOnError(HighBalance);
        else if ( currRecordPtr->balance <= -1 * MAX_AMOUNT) exitOnError(LowBalance);
    }
}

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
    char* descPattern = "^(\\+|\\-)(\t[0-9]{1,10}\t([0-9]{1,7}\\.[0-9]{2})\t)\\w+";
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
        // fprintf(stdout, "opened file: \n");
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
    if (argc > MAX_ARG_COUNT) exitOnErrorCmd(TooManyArgs);
    else if (argc == MAX_ARG_COUNT) {

        if (strcmp(command, argv[1]) != 0) exitOnErrorCmd(UnknownCmd);
        *fileNamePtr = argv[MAX_ARG_COUNT-1];
        exitOnErrorFile(*fileNamePtr); 

     
    }
    else if (argc == MAX_ARG_COUNT - 1) {

        if (strcmp(command, argv[1]) != 0) exitOnErrorCmd(UnknownCmd);
        *inStreamPtr = stdin;
    }

    else exitOnErrorCmd(TooFewArgs); 

    
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

