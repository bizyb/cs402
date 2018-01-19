
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "error.h"
#include "cs402.h"
#include "transaction.h"
#include "warmup1.h"
#include "global.h"

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
