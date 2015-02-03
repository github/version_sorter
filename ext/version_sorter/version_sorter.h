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

#if defined(BUILD_FOR_RUBY)
#include <ruby.h>
#define DIE(msg) \
    rb_raise(rb_eRuntimeError, "%s", msg);
#else
#define DIE(msg) \
    fprintf(stderr, msg);\
    exit(EXIT_FAILURE);
#endif

typedef struct _VersionSortingItem {
    struct _VersionPiece *head;
    struct _VersionPiece *tail;
    int node_len;
    int widest_len;
    char *normalized;
    const char *original;
    size_t original_len;
    int original_idx;
} VersionSortingItem;

typedef struct _VersionPiece {
    char *str;
    int len;
    struct _VersionPiece *next;
} VersionPiece;

enum scan_state {
    digit, alpha, other
};

extern int* version_sorter_sort(char **, size_t);

#endif /* _VERSION_SORTER_H */
