#ifndef list_type
#define list_type

#include <stdbool.h>

typedef struct lelement LElement;
typedef struct llist LList;

//Generic list Element
struct lelement {
    LElement *next;
    LElement *prev;
    void *data;
};

//Generic List Structure
struct llist {
    int NumEl;   //Number of elements in list
    LElement *pFirst;  //Ptr to first element in list
    LElement *pLast;   //Ptr to last element in list
};

void CreateList(LList *list);

LElement *AddToList(void *item, LList *list);

bool DelFromList(unsigned short num, LList * list);

void FreeList(LList * list);

#define FOR_EACH(item_ptr, list) \
     for (item_ptr = list.pFirst; item_ptr != NULL; item_ptr=item_ptr->next)

#endif
