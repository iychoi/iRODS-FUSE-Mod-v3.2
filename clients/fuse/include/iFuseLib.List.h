#ifndef I_FUSE_LIB_LIST_H
#define I_FUSE_LIB_LIST_H

typedef struct fl_listNode fl_ListNode;
struct fl_listNode {
    fl_ListNode *next;
    void *value;
};

typedef struct fl_list {
	int size;
    fl_ListNode *head;
    fl_ListNode *tail;
} fl_List;

typedef struct fl_ConcurrentList {
	fl_List *list;
#ifdef USE_BOOST
    boost::mutex* mutex;
#else
    pthread_mutex_t lock;
#endif
} fl_concurrentList_t;


#ifdef  __cplusplus
extern "C" {
#endif

fl_List *fl_newList();
fl_ListNode *fl_newListNode(void *value);

void fl_listRemove(fl_List *list, fl_ListNode *node);
void fl_listRemove2(fl_List *l, void *v);
void fl_listAppend(fl_List *list, void *value);
void fl_deleteList(fl_List *list);
void fl_clearList(fl_List *list);

fl_concurrentList_t *fl_newConcurrentList();
void fl_addToConcurrentList(fl_concurrentList_t *l, void *v);
void fl_removeFromConcurrentList2(fl_concurrentList_t *l, void *v);
void fl_removeFromConcurrentList(fl_concurrentList_t *l, fl_ListNode *v);
void* fl_removeFirstElementOfConcurrentList(fl_concurrentList_t *l);
void* fl_removeLastElementOfConcurrentList(fl_concurrentList_t *l);
int _fl_listSize(fl_concurrentList_t *l);
int fl_listSize(fl_concurrentList_t *l);
void fl_deleteConcurrentList(fl_concurrentList_t *l);

#ifdef  __cplusplus
}
#endif

#endif

