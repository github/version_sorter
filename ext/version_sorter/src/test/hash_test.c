/*
 *  hash_test.c
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

#include <assert.h>
#include <string.h>
#include "hash.h"


static void
foreach_test_callback(const char *key, void *item, void *state)
{
    int * count_p = (int *) state;
    *count_p = *count_p + 1;
}

void
test_hashtable(void **state)
{
    HashTable *ht = hash_init(2);
    
    hash_put(ht, "hello", "HELLO");
    hash_put(ht, "world", "WORLD");
    
    assert((strcmp("HELLO", (char *) hash_get(ht, "hello"))) == 0);
    assert((strcmp("WORLD", (char *) hash_get(ht, "world"))) == 0);
    
    /* Make sure the foreach works and that I can count the objects */
    int count = 0;
    hash_foreach(ht, &foreach_test_callback, (void *) (&count));
    assert(count == 2);
    
    hash_free(ht);
}