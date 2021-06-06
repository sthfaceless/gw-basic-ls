//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_TYPES_H
#define GW_BASIC_SERVER_TYPES_H

#ifdef WINDOWS
#define LINE_SEPARATOR "\r\n"
#endif
#ifndef LINE_SEPARATOR
#define LINE_SEPARATOR "\n"
#endif

#ifdef WINDOWS
#define VS_CODE_SEPARATOR "\n"
#endif
#ifndef VS_CODE_SEPARATOR
#define VS_CODE_SEPARATOR "\r\n"
#endif

#include "string.h"
#include "stdlib.h"

typedef unsigned long long ull;
typedef long long ll;
typedef long double ld;

typedef enum {
	INT_KEY,
	STRING_KEY,
	DOUBLE_KEY,
	CHAR_KEY,
	STRUCT_KEY
} type_t;

typedef int (* comp)(const void*, const void*);
comp get_comparator(type_t type);
void swap(void** a, void** b);
int* wrapi(int val);
char* wrapc(char val);
void reverse_charr(char* arr, int sz);
ll getmax(ll val1, ll val2);
ll getmin(ll val1, ll val2);
#endif //GW_BASIC_SERVER_TYPES_H
