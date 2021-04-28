//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_TYPES_H
#define GW_BASIC_SERVER_TYPES_H

#include "string.h"

typedef unsigned long long ull;
typedef long long ll;
typedef long double ld;

typedef enum {
    INT,
    STRING,
    DOUBLE,
    CHAR,
    STRUCT
} type_t;

typedef int (*comp)(const void*, const void*);
comp get_comparator(type_t type);
void swap(void* a, void* b);

#endif //GW_BASIC_SERVER_TYPES_H
