/*
*  Author: Bizuwork Melesse
*  USC Spring 2018 - CS402
*  Assignment: Warmup 1
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
 	return TRUE;

}
 int  My402ListLength(My402List* pList) {

 	return pList->num_members;
 }
 int  My402ListEmpty(My402List* pList) {

 	
 	if (pList->num_members == 0) return TRUE;
 	return FALSE;

 }

 int  My402ListAppend(My402List* pList, void* obj)  {

 	My402ListElem *elem = (My402ListElem *) malloc(sizeof(My402ListElem));
 	if (elem == NULL) return FALSE;
 	elem->obj = obj;

 	if (My402ListEmpty(pList)) { 
 		return addToEmpty(pList, elem);
 	}
 	else {
 		My402ListElem *last = My402ListLast(pList);
 		last->next = elem, elem->prev = last;
 		pList->anchor.prev = elem; elem->next = &pList->anchor;

 	}

 	pList->num_members++;
 	return TRUE;
 }
 int  My402ListPrepend(My402List* pList, void* obj) {
 
	My402ListElem *elem = (My402ListElem *) malloc(sizeof(My402ListElem));
	if (elem == NULL) return FALSE;
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
 	return TRUE;
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
 		if (newElem == NULL) return FALSE;
 		newElem->obj = obj;
 		newElem->next = elem->next, elem->next->prev = newElem;
 		elem->next = newElem, newElem->prev = elem;
 		pList->num_members++;
 	}


 	return TRUE;
 }
 int  My402ListInsertBefore(My402List* pList, void* obj, My402ListElem* elem) {

 	if (elem == NULL) {

 		My402ListPrepend(pList, obj);

 	}
 	else {
 		My402ListElem *newElem = (My402ListElem *) malloc(sizeof(My402ListElem));
 		if (newElem == NULL) return FALSE;
 		newElem->obj = obj;
 		newElem->prev = elem->prev, elem->prev->next = newElem;
 		newElem->next = elem, elem->prev = newElem;
 		pList->num_members++;
 	}
 	return TRUE;
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

 	pList->num_members = 0;
 	My402ListElem anchor = {NULL, NULL, NULL};
 	pList->anchor = anchor;
 	return TRUE;
 }

