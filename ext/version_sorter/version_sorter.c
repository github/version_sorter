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
#include <ruby.h>
#include "version_sorter.h"

static pcre *expr;
static VALUE rb_version_sorter_module;

static VersionSortingItem * version_sorting_item_init(const char *);
static void version_sorting_item_free(VersionSortingItem *);
static void version_sorting_item_add_piece(VersionSortingItem *, char *);
static void setup_version_regex(void);
static void parse_version_word(VersionSortingItem *);
static void create_normalized_version(VersionSortingItem *, const int);
static int compare_by_version(const void *, const void *);

static VALUE rb_sort(VALUE, VALUE);
static VALUE rb_rsort(VALUE, VALUE);


void
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

VersionSortingItem *
version_sorting_item_init(const char *original)
{
    VersionSortingItem *vsi = malloc(sizeof(VersionSortingItem));
    if (vsi == NULL) {
        DIE("ERROR: Not enough memory to allocate VersionSortingItem")
    }
    vsi->head = NULL;
    vsi->tail = NULL;
    vsi->node_len = 0;
    vsi->widest_len = 0;
    vsi->original = original;
    vsi->original_len = strlen(original);
	vsi->normalized = NULL;
    parse_version_word(vsi);
    
    return vsi;
}

void
version_sorting_item_free(VersionSortingItem *vsi)
{
    VersionPiece *cur;
    while (cur = vsi->head) {
        vsi->head = cur->next;
		free(cur->str);
        free(cur);
    }
	if (vsi->normalized != NULL) {
		free(vsi->normalized);
	}
    free(vsi);
}

void
version_sorting_item_add_piece(VersionSortingItem *vsi, char *str)
{
    VersionPiece *piece = malloc(sizeof(VersionPiece));
    if (piece == NULL) {
        DIE("ERROR: Not enough memory to allocate string linked list node")
    }
    piece->str = str;
    piece->len = strlen(str);
    piece->next = NULL;
    
    if (vsi->head == NULL) {
        vsi->head = piece;
        vsi->tail = piece;
    } else {
        vsi->tail->next = piece;
        vsi->tail = piece;
    }
    vsi->node_len++;
    if (piece->len > vsi->widest_len) {
        vsi->widest_len = piece->len;
    }
}

void
parse_version_word(VersionSortingItem *vsi)
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

    while (0 < pcre_exec(expr, 0, vsi->original, vsi->original_len, offset, flags, ovector, ovecsize)) {
        
        part = malloc((vsi->original_len+1) * sizeof(char));
        if (part == NULL) {
            DIE("ERROR: Not enough memory to allocate word")
        }

        snprintf(part, vsi->original_len+1, "%.*s", ovector[1]-ovector[0], vsi->original+ovector[0]);

        version_sorting_item_add_piece(vsi, part);
        
        offset = ovector[1];
        flags |= PCRE_NOTBOL;
    }
    
    free(ovector);
}

void
create_normalized_version(VersionSortingItem *vsi, const int widest_len)
{
    VersionPiece *cur;    
    int pos, i;
    
    char *result = malloc(((vsi->node_len * widest_len) + 1) * sizeof(char));
    if (result == NULL) {
        DIE("ERROR: Unable to allocate memory")
    }
    result[0] = '\0';
    pos = 0;
    
    for (cur = vsi->head; cur; cur = cur->next) {
        if (cur->len < widest_len) {
            for (i = 0; i < widest_len - cur->len; i++) {
                result[pos] = ' ';
                pos++;
                result[pos] = '\0';
            }
        }
        strcat(result, cur->str);
        pos += cur->len;
    }
    vsi->normalized = result;
    vsi->widest_len = widest_len;
}

int
compare_by_version(const void *a, const void *b)
{
    return strcmp((*(const VersionSortingItem **)a)->normalized, (*(const VersionSortingItem **)b)->normalized);
}

void
version_sorter_sort(char **list, size_t list_len)
{
    int i, widest_len = 0;
    VersionSortingItem *vsi;
    VersionSortingItem **sorting_list = calloc(list_len, sizeof(VersionSortingItem *));

    for (i = 0; i < list_len; i++) {
        vsi = version_sorting_item_init(list[i]);
        if (vsi->widest_len > widest_len) {
            widest_len = vsi->widest_len;
        }
        sorting_list[i] = vsi;
    }
    
    for (i = 0; i < list_len; i++) {
        create_normalized_version(sorting_list[i], widest_len);
    }

    qsort((void *) sorting_list, list_len, sizeof(VersionSortingItem *), &compare_by_version);
    
    for (i = 0; i < list_len; i++) {
        vsi = sorting_list[i];
        list[i] = (char *) vsi->original;
        
		version_sorting_item_free(vsi);
    }
    free(sorting_list);
}

VALUE
rb_sort(VALUE obj, VALUE list)
{
    long len = RARRAY_LEN(list);
    long i;
    char **c_list = calloc(len, sizeof(char *));
    VALUE rb_str, dest;

    for (i = 0; i < len; i++) {
        rb_str = rb_ary_entry(list, i);
        c_list[i] = StringValuePtr(rb_str);
    }
    version_sorter_sort(c_list, len);

    dest = rb_ary_new2(len);
    for (i = 0; i < len; i++) {
        rb_ary_store(dest, i, rb_str_new2(c_list[i]));
    }

    return dest;
}

VALUE
rb_rsort(VALUE obj, VALUE list)
{
    VALUE dest = rb_sort(obj, list);
    rb_ary_reverse(dest);
    return dest;
}

void
Init_version_sorter(void)
{
    rb_version_sorter_module = rb_define_module("VersionSorter");
    rb_define_module_function(rb_version_sorter_module, "sort", rb_sort, 1);
    rb_define_module_function(rb_version_sorter_module, "rsort", rb_rsort, 1);
}
