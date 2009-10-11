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
#include "hash.h"
#include "version_sorter.h"

#define static

static pcre *expr;
static HashTable *ht;
static int longest_string_len;

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
parse_version_word(char *word, StringLinkedList *sll)
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

    while (0 < pcre_exec(expr, 0, word, strlen(word), offset, flags, ovector, ovecsize)) {
        
        part = malloc((WORD_MAX_LEN+1) * sizeof(char));
        if (part == NULL) {
            DIE("ERROR: Not enough memory to allocate word")
        }

        snprintf(part, WORD_MAX_LEN+1, "%.*s", ovector[1]-ovector[0], word+ovector[0]);

        string_linked_list_append(sll, part);
        
        offset = ovector[1];
        flags |= PCRE_NOTBOL;
    }
    
    free(ovector);
}

static void
foreach_longest_string_callback(const char *key, void *item, void *state)
{
    StringLinkedList *sll = (StringLinkedList *) item;
    StringLinkedListNode *cur;
    int str_len;
    
    cur = sll->head;
    while (cur) {
        str_len = strlen(cur->str);
        if (str_len > longest_string_len) {
            longest_string_len = str_len;
        }
        cur = cur->next;
    }
}

static char *
create_flattened_version(StringLinkedList *sll, int longest_string_len)
{
    StringLinkedListNode *cur;
    int str_len, pos, i;
    
    char *result = malloc(((sll->len * longest_string_len) + 1) * sizeof(char));
    if (result == NULL) {
        DIE("ERROR: Unable to allocate memory")
    }
    result[0] = '\0';
    pos = 0;
    
    for (cur = sll->head; cur; cur = cur->next) {
        str_len = strlen(cur->str);
        if (str_len < longest_string_len) {
            for (i = 0; i < longest_string_len - str_len; i++) {
                result[pos] = ' ';
                pos++;
                result[pos] = '\0';
            }
        }
        strcat(result, cur->str);
        pos += str_len;
    }
    
    return result;
}

static int
compare_by_version(const void *a, const void *b)
{
    int result;
    
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    StringLinkedList *a_sll = (StringLinkedList *) hash_get(ht, *ia);
    StringLinkedList *b_sll = (StringLinkedList *) hash_get(ht, *ib);
    const char *flat_a = create_flattened_version(a_sll, longest_string_len);
    const char *flat_b = create_flattened_version(b_sll, longest_string_len);
    
    result = strcmp(flat_a, flat_b);
    free((void *)flat_a);
    free((void *)flat_b);
    return result;
}

void
version_sorter_sort(char **list, size_t list_len)
{
    int i;
    StringLinkedList *sll;
    ht = hash_init(list_len);

    for (i = 0; i < list_len; i++) {
        sll = string_linked_list_init();
        parse_version_word(list[i], sll);
        hash_put(ht, list[i], (void *) sll);
    }
    
    longest_string_len = 0;
    hash_foreach(ht, &foreach_longest_string_callback, NULL);

    qsort((void *) list, list_len, sizeof(char *), &compare_by_version);
}