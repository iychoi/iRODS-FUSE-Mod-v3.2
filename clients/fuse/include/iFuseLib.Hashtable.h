#ifndef I_FUSE_LIB_HASHTABLE_H
#define I_FUSE_LIB_HASHTABLE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define FL_HASH_BASE 5381
#define fl_myhash(x) fl_B_hash((unsigned char*)(x))

struct fl_bucket {
	char* key;
	void* value;
	struct fl_bucket *next;
};
typedef struct fl_hashtable {
	struct fl_bucket **buckets;
	int size; /* capacity */
	int len;
	int dynamic;
} fl_Hashtable;

struct fl_bucket *fl_newBucket(char* key, void* value);
unsigned long fl_B_hash(unsigned char* string);
fl_Hashtable *fl_newHashTable(int size);
int fl_insertIntoHashTable(fl_Hashtable *h, char* key, void *value);
void* fl_updateInHashTable(fl_Hashtable *h, char* key, void *value);
void* fl_deleteFromHashTable(fl_Hashtable *h, char* key);
void* fl_lookupFromHashTable(fl_Hashtable *h, char* key);
void fl_deleteHashTable(fl_Hashtable *h, void (*f)(void *));
void fl_deleteBucket(struct fl_bucket *h, void (*f)(void *));
struct fl_bucket* fl_lookupBucketFromHashTable(fl_Hashtable *h, char* key);
struct fl_bucket* fl_nextBucket(struct fl_bucket *h, char* key);
void fl_nop(void *a);

#endif
