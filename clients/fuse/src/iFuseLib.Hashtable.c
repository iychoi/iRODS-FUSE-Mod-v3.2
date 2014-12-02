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
#include "iFuseLib.Hashtable.h"

/**
 * returns NULL if out of memory
 */
struct fl_bucket *fl_newBucket(char* key, void* value) {
	struct fl_bucket *b = (struct fl_bucket *)malloc(sizeof(struct fl_bucket));
	if(b==NULL) {
		return NULL;
	}
	b->next = NULL;
	b->key = key;
	b->value = value;
	return b;
}

/**
 * returns NULL if out of memory
 */
fl_Hashtable *fl_newHashTable(int size) {
	fl_Hashtable *h = (fl_Hashtable *)malloc(sizeof (fl_Hashtable));
	if(h==NULL) { // cppcheck - Possible null pointer dereference: h
		return NULL;
	}
	memset(h, 0, sizeof(fl_Hashtable));

	h->dynamic = 0;
	h->size = size;
	h->buckets = (struct fl_bucket **)malloc(sizeof(struct fl_bucket *)*size);
	if(h->buckets == NULL) {
		free(h);
		return NULL;
	}
	memset(h->buckets, 0, sizeof(struct fl_bucket *)*size);
	h->len = 0;
	return h;
}

/**
 * the key is duplicated but the value is not duplicated.
 * MM: the key is managed by hashtable while the value is managed by the caller.
 * returns 0 if out of memory
 */
int fl_insertIntoHashTable(fl_Hashtable *h, char* key, void *value) {
/*
    printf("insert %s=%s\n", key, value==NULL?"null":"<value>");
*/
	if(h->dynamic) {
		if(h->len >= h->size) {
			fl_Hashtable *h2 = fl_newHashTable(h->size * 2);
			int i;
			for(i=0;i<h->size;i++) {
				if(h->buckets[i]!=NULL) {
					struct fl_bucket *b = h->buckets[i];
					do {
						fl_insertIntoHashTable(h2, b->key, b->value);
						b=b->next;

					} while (b!=NULL);
				}
			}
			memcpy(h, h2, sizeof(fl_Hashtable));
		}
		struct fl_bucket *b = fl_newBucket(strdup(key), value);
		if(b==NULL) {
			return 0;
		}

		unsigned long hs = fl_myhash(key);
		unsigned long index = hs%h->size;
		if(h->buckets[index] == NULL) {
			h->buckets[index] = b;
		} else {
			struct fl_bucket *b0 = h->buckets[index];
			while(b0->next!=NULL)
				b0=b0->next;
			b0->next = b;
		}
		h->len ++;
		return 1;
	} else {
		struct fl_bucket *b = fl_newBucket(strdup(key), value);
		if(b==NULL) {
			return 0;
		}
		unsigned long hs = fl_myhash(key);
		unsigned long index = hs % h->size;
		if(h->buckets[index] == NULL) {
			h->buckets[index] = b;
		} else {
			struct fl_bucket *b0 = h->buckets[index];
			while(b0->next!=NULL)
				b0=b0->next;
			b0->next = b;
		}
		h->len ++;
		return 1;
	}
}

/**
 * update hash table returns the pointer to the old value
 */
void* fl_updateInHashTable(fl_Hashtable *h, char* key, void* value) {
	unsigned long hs = fl_myhash(key);
	unsigned long index = hs % h->size;
	if(h->buckets[index] != NULL) {
		struct fl_bucket *b0 = h->buckets[index];
		while(b0!=NULL) {
			if(strcmp(b0->key, key) == 0) {
				void* tmp = b0->value;
				b0->value = value;
				return tmp;
				/* free not the value */
			}
                        b0=b0->next;
		}
	}
	return NULL;
}

/**
 * delete from hash table
 */
void *fl_deleteFromHashTable(fl_Hashtable *h, char* key) {
	unsigned long hs = fl_myhash(key);
	unsigned long index = hs%h->size;
	void *temp = NULL;
	if(h->buckets[index] != NULL) {
            struct fl_bucket *b0 = h->buckets[index];
            if(strcmp(b0->key, key) == 0) {
                    h->buckets[index] = b0->next;
                    temp = b0->value;
                	if(!h->dynamic) {
                    free(b0->key);
                    free(b0);
                	}
                    h->len --;
            } else {
                while(b0->next!=NULL) {
                    if(strcmp(b0->next->key, key) == 0) {
                        struct fl_bucket *tempBucket = b0->next;
                        temp = b0->next->value;
                        b0->next = b0->next->next;
                    	if(!h->dynamic) {
                        free(tempBucket->key);
                        free(tempBucket);
                    	}
                        h->len --;
                        break;
                    }
                    b0 = b0->next;
                }
            }
	}

	return temp;
}

/**
 * returns NULL if not found
 */
void* fl_lookupFromHashTable(fl_Hashtable *h, char* key) {
	unsigned long hs = fl_myhash(key);
	unsigned long index = hs%h->size;
	struct fl_bucket *b0 = h->buckets[index];
	while(b0!=NULL) {
		if(strcmp(b0->key,key)==0) {
			return b0->value;
		}
		b0=b0->next;
	}
	return NULL;
}

/**
 * returns NULL if not found
 */
struct fl_bucket* fl_lookupBucketFromHashTable(fl_Hashtable *h, char* key) {
	unsigned long hs = fl_myhash(key);
	unsigned long index = hs%h->size;
	struct fl_bucket *b0 = h->buckets[index];
	while(b0!=NULL) {
		if(strcmp(b0->key,key)==0) {
			return b0;
		}
		b0=b0->next;
	}
	return NULL;
}

struct fl_bucket* fl_nextBucket(struct fl_bucket *b0, char* key) {
    b0 = b0->next;
    while(b0!=NULL) {
        if(strcmp(b0->key,key)==0) {
            return b0;
        }
        b0=b0->next;
    }
    return NULL;
}

void fl_deleteHashTable(fl_Hashtable *h, void (*f)(void *)) {
	if(h->dynamic) {
		/*if(f != NULL) {
			int i;
			for(i =0;i<h->size;i++) {
				struct bucket *b0 = h->buckets[i];
				while(b0!=NULL) {
					f(b0->value);
					b0= b0->next;
				}
			}

		}*/
	} else {
		int i;
		for(i =0;i<h->size;i++) {
			struct fl_bucket *b0 = h->buckets[i];
			if(b0!=NULL)
				fl_deleteBucket(b0,f);
		}
		free(h->buckets);
		free(h);
	}
}

void fl_deleteBucket(struct fl_bucket *b0, void (*f)(void *)) {
		if(b0->next!=NULL) {
			fl_deleteBucket(b0->next, f);
		}
                /* todo do not delete keys if they are allocated in regions */
                free(b0->key);
		if(f!=NULL) f(b0->value);
		free(b0);
}

void fl_nop(void *a) {
}

unsigned long fl_B_hash(unsigned char* string) { /* Bernstein hash */
	unsigned long hash = FL_HASH_BASE;
	while(*string!='\0') {
	    hash = ((hash << 5) + hash) + (int)*string;
        string++;
	}
	return hash;

}


