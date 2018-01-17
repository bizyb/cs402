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
#include <time.h>

#include "cs402.h"
#include "my402list.h"
#include "warmup1.h"



// static char gszProgName[MAXPATHLENGTH];
const int BUFFERSIZE = 1025;
const double FACTOR = 100.00;
const int MAX_AMOUNT = 1000000000; // $10,000,000.00
const int MAX_DATE_LENGTH = 10;
const int BASE = 10;






/* ----------------------- Utility Functions -----------------------*/
void printError(ErrorType e) {

    // TODO: do we have to handle line length?
    char* msg;
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
        case Malloc: msg = "Failed to allocate memory"; break;
        case ListInsertion: msg = "Failed to add transaction record into list.";
        default: msg = "Unknown error";
    }

    fprintf(stderr, "%s Exting program...\n", msg);
    exit(1);

}

void printHistory(My402List*  pList) {

    // TODO: Format floats to display with commas as appropriate
    // TODO: add ???.. for amount/balance exceeding 10mil
    // TODO: date formatting

    My402ListElem* curr = pList->anchor.next;
    char* border = "+-----------------+--------------------------+----------------+----------------+";
    fprintf(stdout, "%s\n", border);
    fprintf(stdout, "|       Date      ");
    fprintf(stdout, "| Description              ");
    fprintf(stdout, "|         Amount ");
    fprintf(stdout, "|        Balance |\n");
    fprintf(stdout, "%s\n", border);


    while (curr != NULL) {
        Transaction record = *(Transaction* ) curr->obj;

        fprintf(stdout, "| %s ", record.date);
        fprintf(stdout, "| %s", record.desc);

        char* amount = formatCurrency(record.amount);
        if (record.flag == WITHDRAWAL) fprintf(stdout, "| (%s) ", amount);
        else fprintf(stdout, "|  %s  ", formatCurrency(record.amount));
        
        amount = formatCurrency(record.amount);
        if (record.flag == WITHDRAWAL) fprintf(stdout, "| (%s) |\n", amount);
        else fprintf(stdout, "|  %s  |\n", formatCurrency(record.amount));

        curr = My402ListNext(pList, curr);
        // free(amount);
    }
    fprintf(stdout, "%s\n", border);

}


Flag getFlag(char* flag) {

    Flag tFlag;

    if (*flag == '-') tFlag = WITHDRAWAL;
    else if (*flag == '+') tFlag = DEPOSIT;
    else {
        ErrorType e = UnknownFlag;
        printError(e);
    }
    return tFlag;
}
int getAmount(char* amount) {

    char* end;
    // fprintf(stdout, "before conversion amount: %s\n", amount);
    double dAmount = strtod(amount, &end);
    if (amount == end) {
        // if conversion fails, address of amount gets stored at address of end
        // fprintf(stdout, "failed on amount: %s\n", amount);
        ErrorType e = UnknownAmount;
        printError(e);
    }

    int iAmount = round(dAmount * FACTOR);
    // fprintf(stdout, "dAmount: %f\n", dAmount);
    // fprintf(stdout, "dAmount*  FACTOR: %f\n", dAmount*  FACTOR);
    // fprintf(stdout, "iAmount = (int) (dAmount*  FACTOR): %d\n", iAmount);
    if (iAmount >= MAX_AMOUNT) {

        ErrorType e = HighAmount;
        printError(e);
    }
    return iAmount;

}

int getRawDate(char* date) {

    unsigned int length = strlen(date);
    char* end;
    int iDate = strtol(date, &end, BASE);
    if (length > MAX_DATE_LENGTH || date == end) {

        ErrorType e = InvalidTimestamp;
        printError(e);
    }

    return iDate;
}

char* getDesc(char* desc) {

    unsigned int length = strlen(desc);
    const unsigned int DESC_LENGTH = 23;
    const unsigned int CELL_LENGTH = 26;

    // TODO: This will lead to some leakage so need to do something about it
    char * abridged = (char *) malloc((DESC_LENGTH +1)*sizeof(char)); 

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
    char* buffer = (char* ) malloc((DATE_LENGTH +1) *sizeof(char)); //TODO: malloc needs to be freed
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
   
    char* buffer = (char*) malloc((FIELD_WIDTH+1)*sizeof(char)); //TODO: free up the memory in printHistory
    double fAmount = (value / FACTOR);
    // fprintf(stdout, "fAmount first: %f\n", fAmount);
    sprintf(buffer, "%.2f", fAmount);
    int newLength = insertComma(buffer);

    // fprintf(stdout, "--------------------------------------\n");
    // fprintf(stdout, "value: %d\n", value);
    // fprintf(stdout, "value / (float)FACTOR: %f\n", value / (float)FACTOR);
    //  fprintf(stdout, "fAmount second: %f\n", fAmount);
    // fprintf(stdout, "buffer: %s\n", buffer);
    // fprintf(stdout, "--------------------------------------\n");

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

    // char *temp = (char*) malloc((FIELD_WIDTH+1)*sizeof(char));
    char temp[FIELD_WIDTH];
    char c = ',';
    memset(&temp, 0, (FIELD_WIDTH+1)*sizeof(char));
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

    return length;
    // free(temp);


}
Transaction getTransaction(char* flag, char* date, char* amount, char* desc) {

    char* tDate,* tDesc;
    int tAmount, tBalance, tRawDate;
    Flag tFlag;

    tFlag = getFlag(flag);
    tRawDate = getRawDate(date);
    tDate = formatDate(tRawDate); // no need to do error handling since getDate() succeeded
    tDesc = getDesc(desc); //, check that it's not empty
   
    tAmount = getAmount(amount);

    tBalance = 0; //for now; will be computed after sorting; when iterating through the sorted list, 
    //keep local max abs(balance). Handle >=10mil accordingly 

    // fprintf(stdout, "in transaction: %s\n", tDesc);
    Transaction record = {tDate, tDesc, tAmount, tBalance, tRawDate, tFlag};
    return record;
}

Transaction* copyTransaction(Transaction record) {

    // TODO: check all malloc and memcpy return values

    Transaction* recPtr = (Transaction* ) malloc(sizeof(Transaction));
    recPtr->date = (char* ) malloc(sizeof(char));
    recPtr->desc = (char* ) malloc(sizeof(char));
    recPtr->amount = *(int* ) malloc(sizeof(int));
    recPtr->balance = *(int* ) malloc(sizeof(int));
    recPtr->dateRaw = *(int* ) malloc(sizeof(int));
    recPtr->flag = *(Flag* ) malloc(sizeof(Flag));

    (void)memcpy((void* ) recPtr->date, (void* ) record.date, (strlen(record.date)+1)*sizeof(char));
    (void)memcpy((void* ) recPtr->desc, (void* ) record.desc, (strlen(record.desc)+1)*sizeof(char));
    (void)memcpy((void* ) &recPtr->amount, (void* ) &record.amount, sizeof(int)+1);
    (void)memcpy((void* ) &recPtr->balance, (void* ) &record.balance, sizeof(int)+1);
    (void)memcpy((void* ) &recPtr->dateRaw, (void* ) &record.dateRaw, sizeof(int)+1);
    (void)memcpy((void* ) &recPtr->flag, (void* ) &record.flag, sizeof(Flag)+1);
    

    // if (recPtr == NULL) {
    //     ErrorType e = Malloc;
    //     printError(e);
    // }
    return recPtr;

}

void insertTransaction(My402List* pList, Transaction record) {

    //iterate through the list and insert the record in a sorted order
    // if another record with the same timestamp exists, call printError to exit the program
   
    int result = My402ListAppend(pList, (void* ) copyTransaction(record));
    if (result == FALSE) {

        ErrorType e = ListInsertion;
        printError(e);
    }
    
    
}
int validateFormat(char* flag, char* date, char* amount, char* desc) {

    // TODO: validate input data
    return TRUE;
}

Transaction parseLine(char* line) {

    char* delim = "\t";
    char* flag,* date,* amount,* desc;
    flag = strtok(line, delim);
    date = strtok(NULL, delim);
    amount = strtok(NULL, delim);
    desc = strtok(NULL, delim);


    if (!validateFormat(flag, date, amount, desc)) {
        ErrorType e = MalformedLine;
        printError(e);
    }
    if (desc[strlen(desc)-1] == '\n')  desc[strlen(desc)-1] = '\0';

    return getTransaction(flag, date, amount, desc);
}

int readInput(My402List* pList, char* path) {

    char buffer [BUFFERSIZE];
    FILE* file;

    file = fopen(path, "r");
    if (file == NULL) {
        ErrorType e = FileOpen;
        printError(e);
    }
    while (fgets(buffer, BUFFERSIZE,  file) != NULL) {
        Transaction record = parseLine((char* ) &buffer);
        insertTransaction(pList, record);
    }

    return TRUE;

}

/* ----------------------- main() -----------------------*/

int main(int argc, char* argv[])
{
    
    char* path = "tfile.txt";
    My402List list;
    memset(&list, 0, sizeof(My402List));
    (void)My402ListInit(&list);

    readInput(&list, path);
    printHistory(&list);



    // free all memory
    // My402ListUnlinkAll(&list);
    
    return(0);
}
