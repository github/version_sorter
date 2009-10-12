/*
 *  version_sorter.h
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#ifndef _VERSION_SORTER_H
#define _VERSION_SORTER_H

#define WORD_MAX_LEN 100

typedef struct _SortingItem {
    int original_index;
    StringLinkedList *sll;
} SortingItem;

extern void version_sorter_sort(char **, size_t);

#endif /* _VERSION_SORTER_H */