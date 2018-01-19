
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <sys/time.h>

#include <time.h>

#include "cs402.h"
#include "transaction.h"
#include "my402list.h"
#include "global.h"
#include "history.h"

char* cleanString(char* dirtyString) {

    unsigned int i;
    for (i = 0; i < strlen(dirtyString); i++) {

        if (dirtyString[i] == '\n') dirtyString[i] = ' ';

    }
    return dirtyString;
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

void freeMemory(My402List* pList, My402ListElem* elem) {
    
    Transaction* recPtr = (Transaction*) elem->obj;
    free(recPtr->date);
    free(recPtr->desc);
    free(recPtr);
    My402ListUnlink(pList, elem);

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