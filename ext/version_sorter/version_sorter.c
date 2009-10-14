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
#include "version_sorter.h"


static VersionSortingItem * version_sorting_item_init(const char *);
static void version_sorting_item_free(VersionSortingItem *);
static void version_sorting_item_add_piece(VersionSortingItem *, char *);
static void parse_version_word(VersionSortingItem *);
static void create_normalized_version(VersionSortingItem *, const int);
static int compare_by_version(const void *, const void *);
static enum scan_state scan_state_get(const char);


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

enum scan_state
scan_state_get(const char c)
{
    if (isdigit(c)) {
        return digit;
    } else if (isalpha(c)) {
        return alpha;
    } else {
        return other;
    }

}

void
parse_version_word(VersionSortingItem *vsi)
{
    int start = 0, end = 0, size = 0;
    char current_char, next_char;
    char *part;
    enum scan_state current_state, next_state;
    
    while ((current_char = vsi->original[start]) != '\0') {
        current_state = scan_state_get(current_char);
        
        if (current_state == other) {
            start++;
            end = start;
            continue;
        }
        
        do {
            end++;
            next_char = vsi->original[end];
            next_state = scan_state_get(next_char);
        } while (next_char != '\0' && current_state == next_state);

        size = end - start;
        
        part = malloc((size+1) * sizeof(char));
        if (part == NULL) {
            DIE("ERROR: Not enough memory to allocate word")
        }
        
        memcpy(part, vsi->original+start, size);
        part[size] = '\0';
        
        version_sorting_item_add_piece(vsi, part);
        
        start = end;
    }
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
        
        /* Left-Pad digits with a space */
        if (cur->len < widest_len && isdigit(cur->str[0])) {
            for (i = 0; i < widest_len - cur->len; i++) {
                result[pos] = ' ';
                pos++;
            }
            result[pos] = '\0';
        }
        strcat(result, cur->str);
        pos += cur->len;

        /* Right-Pad words with a space */
        if (cur->len < widest_len && isalpha(cur->str[0])) {
            for (i = 0; i < widest_len - cur->len; i++) {
                result[pos] = ' ';
                pos++;
            }
            result[pos] = '\0';
        }
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