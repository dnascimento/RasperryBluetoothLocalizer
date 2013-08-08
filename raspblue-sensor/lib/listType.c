#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "listType.h"
#include "discovery.h"

void CreateList(LList *list)
{
    list->NumEl = 0;
    list->pFirst = NULL;
    list->pLast = NULL;
}

LElement *AddToList(void *item, LList *list)
{
    //check inputs
    assert(item!=NULL); 
    assert(list!=NULL);
    //Create generic element to hold item ptr
    LElement *NewEl;
    NewEl = (LElement *)malloc(sizeof(LElement));  //create generic element
    assert(NewEl != NULL);
    list->NumEl = list->NumEl + 1;
    NewEl->data = item;
    if (list->NumEl == 1)
    {
      list->pFirst = NewEl;
      NewEl->prev = NULL;
      NewEl->next = NULL;
    }
    else
    {
      NewEl->prev = list->pLast;
      NewEl->next = NULL;
      list->pLast->next = NewEl;
    }
    list->pLast = NewEl;
    return NewEl;
}

bool DelFromList(unsigned short num, LList * list){
	unsigned short i;
	LElement *curr;
	bool deleted = false;
	//check inputs
    assert(num <= list->NumEl); 
    assert(list!=NULL);
    
    if (list->pFirst == NULL)
		return true;
		
    for (i=1, curr=list->pFirst; i <= num; i++, curr=curr->next){
		if (i == num){
			if (curr->next != NULL && curr->prev != NULL){
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
			}else if(curr->prev != NULL && curr->next == NULL){
				curr->prev->next = NULL;
			}else if (curr->prev == NULL && curr->next != NULL){
				curr->next->prev = NULL;
			}
			
			if (list->NumEl == 1){
				list->pFirst = NULL;
				list->pLast = NULL;
			}else{
				if (curr == list->pFirst)
					list->pFirst = curr->next;
			
				if (curr == list->pLast)
					list->pLast = curr->prev;
			}
			
			free(curr);
			deleted = true;
		}
	}
	
	if (deleted)
		list->NumEl = list->NumEl - 1;
		
	return deleted;
}

void FreeList(LList * list){
	unsigned short i;
	short j = list->NumEl;
	debugger("FREELIST: %d\n", j);
	for(i=1; i <= j; i++){
		debugger("INSIDE FREELIST: %d\n",i);
		DelFromList(1,list);
	}
}
