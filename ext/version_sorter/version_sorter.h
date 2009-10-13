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

#if UNIT_TESTING

#define static

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#if XCODE
#include <cmockery/cmockery.h>
#else
#include <cmockery.h>
#endif

#endif

#define DIE(msg) \
    fprintf(stderr, msg);\
    exit(EXIT_FAILURE);\

typedef struct _VersionSortingItem {
    struct _VersionPiece *head;
    struct _VersionPiece *tail;
    int node_len;
    int widest_len;
    char *normalized;
    const char *original;
    size_t original_len;
} VersionSortingItem;

typedef struct _VersionPiece {
    char *str;
    int len;
    struct _VersionPiece *next;
} VersionPiece;

enum scan_state {
    digit, alpha, other
};

extern void version_sorter_sort(char **, size_t);

#endif /* _VERSION_SORTER_H */