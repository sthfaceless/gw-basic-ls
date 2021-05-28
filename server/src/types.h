//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_TYPES_H
#define GW_BASIC_SERVER_TYPES_H

#include "string.h"
#include "stdlib.h"

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

typedef int (* comp)(const void*, const void*);
comp get_comparator(type_t type);
void swap(void** a, void** b);
int* wrapi(int val);
char* wrapc(char val);
void reverse_charr(char* arr, int sz);
ll max(ll val1, ll val2);
ll min(ll val1, ll val2);
#endif //GW_BASIC_SERVER_TYPES_H
