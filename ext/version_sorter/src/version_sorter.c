/*
 *  version_sorter.c
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pcre.h>
#include "strings.h"
#include "utils.h"
#include "version_sorter.h"

#define static

static pcre *expr;

static void
setup_version_regex(void)
{
    const char *pattern = "(\\d+|[a-zA-Z]+)";
    const char *errstr;
    int erroffset;
    
    if (!(expr = pcre_compile(pattern, 0, &errstr, &erroffset, 0))) {
        fprintf(stderr, "ERROR: %s: %s\n", pattern, errstr);
        exit(EXIT_FAILURE);
    }    
}

static void
parse_version_word(StringLinkedList *sll)
{
    if (expr == NULL) {
        setup_version_regex();
    }
    
    int ovecsize = 0;
    if (pcre_fullinfo(expr, NULL, PCRE_INFO_CAPTURECOUNT, &ovecsize) != 0) {
        DIE("ERROR: Problem calling pcre_fullinfo")
    }
    ovecsize = (ovecsize + 1) * 3;
    
    int *ovector = calloc(ovecsize, sizeof(int));
    if (ovector == NULL) {
        DIE("ERROR: Not enough memory to allocate ovector")
    }

    int offset = 0;
    int flags = 0;
    char *part;

    while (0 < pcre_exec(expr, 0, sll->original, strlen(sll->original), offset, flags, ovector, ovecsize)) {
        
        part = malloc((WORD_MAX_LEN+1) * sizeof(char));
        if (part == NULL) {
            DIE("ERROR: Not enough memory to allocate word")
        }

        snprintf(part, WORD_MAX_LEN+1, "%.*s", ovector[1]-ovector[0], sll->original+ovector[0]);

        string_linked_list_append(sll, part);
        
        offset = ovector[1];
        flags |= PCRE_NOTBOL;
    }
    
    free(ovector);
}

static void
create_normalized_version(StringLinkedList *sll, const int widest_len)
{
    StringLinkedListNode *cur;    
    int str_len, pos, i;
    
    char *result = malloc(((sll->len * widest_len) + 1) * sizeof(char));
    if (result == NULL) {
        DIE("ERROR: Unable to allocate memory")
    }
    result[0] = '\0';
    pos = 0;
    
    for (cur = sll->head; cur; cur = cur->next) {
        str_len = strlen(cur->str);
        if (str_len < widest_len) {
            for (i = 0; i < widest_len - str_len; i++) {
                result[pos] = ' ';
                pos++;
                result[pos] = '\0';
            }
        }
        strcat(result, cur->str);
        pos += str_len;
    }
    sll->normalized = result;
    sll->widest_len = widest_len;
}

static int
compare_by_version(const void *a, const void *b)
{
    return strcmp((*(const SortingItem **)a)->sll->normalized, (*(const SortingItem **)b)->sll->normalized);
}

void
version_sorter_sort(char **list, size_t list_len)
{
    int i;
    StringLinkedList *sll;
    static int longest_string_len = 0;
    SortingItem **sorting_list = calloc(list_len, sizeof(SortingItem *));
    SortingItem *sorting_item;

    for (i = 0; i < list_len; i++) {
        sll = string_linked_list_init(list[i]);
        parse_version_word(sll);
        if (sll->widest_len > longest_string_len) {
            longest_string_len = sll->widest_len;
        }
        sorting_item = malloc(sizeof(SortingItem));
        sorting_item->sll = sll;
        sorting_item->original_index = i;
        sorting_list[i] = sorting_item;
    }
    
    for (i = 0; i < list_len; i++) {
        create_normalized_version(sorting_list[i]->sll, longest_string_len);
    }

    qsort((void *) sorting_list, list_len, sizeof(SortingItem *), &compare_by_version);
    
    for (i = 0; i < list_len; i++) {
        sorting_item = sorting_list[i];
        list[i] = (char *) sorting_item->sll->original;
        
        string_linked_list_free(sorting_item->sll);
        free(sorting_item);
    }
    free(sorting_list);
}