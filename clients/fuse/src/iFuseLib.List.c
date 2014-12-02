#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include "irodsFs.h"
#include "iFuseLib.h"
#include "iFuseOper.h"
#include "miscUtil.h"
#include "iFuseLib.List.h"

fl_List *fl_newList() {
	fl_List *l = (fl_List *)malloc(sizeof (fl_List));
	    l->head = l->tail = NULL;
	    l->size = 0;
	    return l;
}

fl_ListNode *fl_newListNode(void *value) {
    fl_ListNode *l = (fl_ListNode *)malloc(sizeof (fl_ListNode));
    l->next = NULL;
    l->value = value;
    return l;
}

void fl_listRemove(fl_List *list, fl_ListNode *node) {
    fl_ListNode *prev = NULL, *curr = list->head;
    while(curr != NULL) {
        if(curr == node) {
            if(prev == NULL) {
                list->head = node->next;
            } else {
                prev->next = node->next;
            }
            free(node);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if(list->tail == node) {
        list->tail = prev;
    }
    list->size--;
}

void fl_listRemove2(fl_List *l, void *v) {
	fl_ListNode *node = l->head;
	while(node!=NULL) {
		if(node->value == v) {
			fl_listRemove(l, node);
			break;
		}
		node = node->next;
	}
}

void fl_listAppend(fl_List *list, void *value) {
    fl_ListNode *ln = fl_newListNode(value);
    if(list->head != NULL) {
        list->tail = list->tail->next = ln;
    } else {
        list->head = list->tail = ln;

    }
    list->size++;
}

void fl_deleteList(fl_List *list) {
	free(list);
}

void fl_clearList(fl_List *list) {
	while(list->head != NULL)
        fl_listRemove(list, list->head);
}


fl_concurrentList_t *fl_newConcurrentList() {
	fl_List *l = fl_newList();
	fl_concurrentList_t *cl = (fl_concurrentList_t *) malloc(sizeof (fl_concurrentList_t));
	cl->list = l;
	INIT_STRUCT_LOCK(*cl);
	return cl;
}

void fl_addToConcurrentList(fl_concurrentList_t *l, void *v) {
	LOCK_STRUCT(*l);
	fl_listAppend(l->list, v);
	UNLOCK_STRUCT(*l);
}

void fl_removeFromConcurrentList2(fl_concurrentList_t *l, void *v) {
	LOCK_STRUCT(*l);
	fl_listRemove2(l->list, v);
	UNLOCK_STRUCT(*l);

}

void fl_removeFromConcurrentList( fl_concurrentList_t *l, fl_ListNode *v ) {
    LOCK_STRUCT( *l );
    fl_listRemove(l->list, v);
    UNLOCK_STRUCT(*l);
}

void* fl_removeFirstElementOfConcurrentList(fl_concurrentList_t *l) {
	LOCK_STRUCT(*l);
	void* tmp;
	if(l->list->head == NULL) {
		tmp = NULL;
    }
    else {
		tmp = l->list->head->value;
		fl_listRemove(l->list, l->list->head);
	}
	UNLOCK_STRUCT(*l);
	return tmp;
}

void *fl_removeLastElementOfConcurrentList(fl_concurrentList_t *l) {
	LOCK_STRUCT(*l);
	void* tmp;
	if(l->list->head == NULL) {
		tmp = NULL;
    }
    else {
		tmp = l->list->tail->value;
		fl_listRemove(l->list, l->list->tail);
	}
	UNLOCK_STRUCT(*l);
	return tmp;

}

int _fl_listSize( fl_concurrentList_t *l ) {
    return l->list->size;
}

int fl_listSize(fl_concurrentList_t *l) {
	LOCK_STRUCT(*l);
	int s = _fl_listSize(l);
	UNLOCK_STRUCT(*l);
	return s;
}

void fl_deleteConcurrentList(fl_concurrentList_t *l) {
    LOCK_STRUCT(*l);
    fl_clearList(l->list);
	fl_deleteList(l->list);
    UNLOCK_STRUCT(*l);
	free(l);
}

