/*
 *  hash.c
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include "hash.h"

/* The SDBM hash */
static unsigned int
str_hash(const char *str)
{
    unsigned int hash = 0;
    int c;
    
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

HashTable *
hash_init(unsigned int size)
{
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        DIE("ERROR: Not enough memory to allocate HashTable")
    }

    /* The capacity should be 20% larger than the size.  I am using this int
     * because I want small numbers to have a larger size by at least 1
     */
    unsigned int capacity = ceil(size * 1.2);
    /* The capacity needs to be even */
    if ((capacity % 2) == 1) {
        capacity++;
    }
    
    ht->capacity = capacity;
    ht->size = 0;
    
    ht->table = calloc(capacity, sizeof(HashTableItem *));
    if (ht->table == NULL) {
        DIE("ERROR: Not enough memory to allocate HashTable lookup")
    }
    
    return ht;
}

void
hash_free(HashTable *ht)
{
    HashTableItem *cur, *next;
    int i;
    
    for (i = 0; i < ht->capacity; i++) {
        cur = ht->table[i];
        while (cur) {
            next = cur->next;
            free(cur);
            cur = next;
        }
    }
    free(ht->table);
    free(ht);
}

void *
hash_get(HashTable * ht, const char *key)
{
    unsigned int index = str_hash(key) % (ht->capacity - 1);
    HashTableItem *cur;
    cur = ht->table[index];
    while (cur) {
        if ((strcmp(key, cur->key)) == 0) {
            return cur->item;
        }
        cur = cur->next;
    }
    
    return NULL;
}

void
hash_put(HashTable *ht, const char *key, void * item)
{
    unsigned int index = str_hash(key) % (ht->capacity - 1);
    HashTableItem *entry = malloc(sizeof(HashTableItem));
    if (entry == NULL) {
        DIE("ERROR: Not enough memory to allocate HashTableItem")
    }
    entry->key = key;
    entry->item = item;
    entry->next = ht->table[index];
    ht->table[index] = entry;
}

void
hash_foreach(HashTable *ht, hash_iter_cb block, void *state)
{
    HashTableItem *cur, *next;
    int i;
    
    for (i = 0; i < ht->capacity; i++) {
        cur = ht->table[i];
        while (cur) {
            next = cur->next;
            block(cur->key, cur->item, state);
            cur = next;
        }
    }
}