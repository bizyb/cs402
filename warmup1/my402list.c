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
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: listtest.c,v 1.2 2018/01/02 23:09:34 william Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"

#include "my402list.h"

 
int addToEmpty(My402List* pList, My402ListElem *elem) {

	pList->anchor.prev = elem, pList->anchor.next = elem;
 	elem->prev = &pList->anchor, elem->next = &pList->anchor;
 	pList->num_members++; 
 	return 1;

}
 int  My402ListLength(My402List* pList) {

 	return pList->num_members;
 }
 int  My402ListEmpty(My402List* pList) {

 	// return pList->anchor.prev == NULL && pList->anchor.next == NULL;
 	return pList->num_members == 0;

 }

 int  My402ListAppend(My402List* pList, void* obj)  {

 	// TODO: malloc errors


 	My402ListElem *elem = (My402ListElem *) malloc(sizeof(My402ListElem));
 	elem->obj = obj;

 	if (My402ListEmpty(pList)) {
 		// pList->anchor.prev = elem, pList->anchor.next = elem;
 		// elem->prev = &pList->anchor, elem->next = &pList->anchor; 
 		return addToEmpty(pList, elem);
 	}
 	else {
 		My402ListElem *last = My402ListLast(pList);
 		last->next = elem, elem->prev = last;
 		pList->anchor.prev = elem; elem->next = &pList->anchor;

 	}
 	pList->num_members++;
 	return 1;
 }
 int  My402ListPrepend(My402List* pList, void* obj) {

 	// TODO: malloc errors

 	My402ListElem *elem = (My402ListElem *) malloc(sizeof(My402ListElem));
 	elem->obj = obj;
 	if (My402ListEmpty(pList)) {

 		return addToEmpty(pList, elem);
 	}
 	else {
 		My402ListElem *first = My402ListFirst(pList);


 		first->prev = elem, elem->next = first;
 		pList->anchor.next = elem, elem->prev = &pList->anchor; 
 	}
 	pList->num_members++;
 	return 1;
 }
 void My402ListUnlink(My402List* pList, My402ListElem* elem) {

 	elem->prev->next = elem->next;
 	elem->next->prev = elem->prev;
 	pList->num_members--;
 	free(elem);


 }
 void My402ListUnlinkAll(My402List* pList) {

 	if (!My402ListEmpty(pList)) {

 		My402ListElem *curr = pList->anchor.next;
 		curr->prev = NULL;
 		pList->anchor.next = NULL, pList->anchor.prev->next = NULL;
 		pList->anchor.prev=NULL;


 		while(curr != NULL) {
 			My402ListElem *prev = curr;
 			curr = curr->next;
 			free(prev);
 			pList->num_members--;
 		}

 	}

 }
 int  My402ListInsertAfter(My402List* pList, void* obj, My402ListElem* elem) {

 	if (elem == NULL) {

 		My402ListAppend(pList, obj);

 	}
 	else {
 		My402ListElem *newElem = (My402ListElem *) malloc(sizeof(My402ListElem));
 		newElem->obj = obj;
 		newElem->next = elem->next, elem->next->prev = newElem;
 		elem->next = newElem, newElem->prev = elem;
 		pList->num_members++;
 	}


 	return 1;
 }
 int  My402ListInsertBefore(My402List* pList, void* obj, My402ListElem* elem) {

 	if (elem == NULL) {

 		My402ListPrepend(pList, obj);

 	}
 	else {
 		My402ListElem *newElem = (My402ListElem *) malloc(sizeof(My402ListElem));
 		newElem->obj = obj;
 		newElem->prev = elem->prev, elem->prev->next = newElem;
 		newElem->next = elem, elem->prev = newElem;
 		pList->num_members++;
 	}
 	return 1;
 }

 My402ListElem *My402ListFirst(My402List* pList) {

 	return pList->anchor.next;
 }
 My402ListElem *My402ListLast(My402List* pList) {

 	return pList->anchor.prev;
 }
 My402ListElem *My402ListNext(My402List* pList, My402ListElem* elem) {

 	if (elem->next != &pList->anchor) return elem->next;
 	return NULL;
 }
 My402ListElem *My402ListPrev(My402List* pList, My402ListElem* elem) {

 	if (elem->prev != &pList->anchor) return elem->prev;
 	return NULL;
 }

 My402ListElem *My402ListFind(My402List* pList, void* obj) {

 	My402ListElem *elem = NULL;

 	for (elem = My402ListFirst(pList); elem != NULL; elem = My402ListNext(pList, elem)) {

 		void *target = (void *) elem->obj;
 		if (target == obj) return elem;
 	}

 	return NULL;
 }

 
 int My402ListInit(My402List *pList) {

 	pList = (My402List *) malloc(sizeof(My402List));
 	pList->num_members = 0;
 	My402ListElem anchor = {NULL, NULL, NULL};
 	pList->anchor = anchor;

 	return 1;
 }