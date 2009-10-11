/*
 *  string.h
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#ifndef _STRING_H
#define _STRING_H

typedef struct _StringLinkedList {
    struct _StringLinkedListNode *head;
    struct _StringLinkedListNode *tail;
    int len;
} StringLinkedList;

typedef struct _StringLinkedListNode {
    char *str;
    struct _StringLinkedListNode *next;
} StringLinkedListNode;

extern int string_cmp(const void *, const void *);

extern StringLinkedList * string_linked_list_init(void);
extern void string_linked_list_free(StringLinkedList *);
extern void string_linked_list_append(StringLinkedList *, char *);

#endif /* _STRING_H */