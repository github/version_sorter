/*
 *  hash.h
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#ifndef _HASH_H
#define _HASH_H

typedef struct _HashTable {
    unsigned int capacity;
    unsigned int size;
    struct _HashTableItem **table;
} HashTable;

typedef struct _HashTableItem {
    const char *key;
    void *item;
    struct _HashTableItem *next;
} HashTableItem;

typedef void (*hash_iter_cb)(const char *, void *, void *);

extern HashTable * hash_init(unsigned int);
extern void hash_free(HashTable *);
extern void * hash_get(HashTable *, const char*);
extern void hash_put(HashTable *, const char *, void *);
extern void hash_foreach(HashTable *, hash_iter_cb, void *);

#endif /* _HASH_H */