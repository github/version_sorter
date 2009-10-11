/*
 *  utils.h
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define DIE(msg) \
    fprintf(stderr, msg);\
    exit(EXIT_FAILURE);\

#define ARRAY_LENGH(x) \
    (sizeof(x)/sizeof(x[0]))

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#endif /* _UTILS_H */