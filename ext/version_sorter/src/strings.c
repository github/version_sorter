/*
 *  string.c
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "utils.h"

int
string_cmp(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

StringLinkedList *
string_linked_list_init(const char *original)
{
    StringLinkedList *sll = malloc(sizeof(StringLinkedList));
    if (sll == NULL) {
        DIE("ERROR: Not enough memory to allocate StringLinkedList")
    }
    sll->head = NULL;
    sll->tail = NULL;
    sll->len = 0;
    sll->widest_len = 0;
    sll->normalized = NULL;
    sll->original = original;
    
    return sll;
}

void
string_linked_list_free(StringLinkedList *sll)
{
    StringLinkedListNode *cur;
    while (cur = sll->head) {
        sll->head = cur->next;
        free(cur->str);
        free(cur);
    }
    if (sll->normalized != NULL) {
        free(sll->normalized);
    }
    free(sll);
}

void
string_linked_list_append(StringLinkedList *sll, char *str)
{
    StringLinkedListNode *slln = malloc(sizeof(StringLinkedListNode));
    if (slln == NULL) {
        DIE("ERROR: Not enough memory to allocate string linked list node")
    }
    slln->str = str;
    slln->len = strlen(str);
    slln->next = NULL;
    
    if (sll->head == NULL) {
        sll->head = slln;
        sll->tail = slln;
    } else {
        sll->tail->next = slln;
        sll->tail = slln;
    }
    sll->len++;
    if (slln->len > sll->widest_len) {
        sll->widest_len = slln->len;
    }
}