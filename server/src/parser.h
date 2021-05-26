//
// Created by danil on 01.05.2021.
//

#ifndef GW_BASIC_SERVER_PARSER_H
#define GW_BASIC_SERVER_PARSER_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "cstring.h"
#include "json.h"
#include "json-builder.h"
#include "json_helper.h"
#include "logger.h"
#include "map.h"
#include "diagnostic_messages.h"

typedef enum {
	Unknown,
	File,
	Module,
	Namespace,
	Package,
	Class,
	Method,
	Property,
	Field,
	Constructor,
	Enum,
	Interface,
	Function,
	Variable,
	Constant,
	String,
	Number,
	Boolean,
	Array,
	Object,
	Key,
	Null,
	EnumMember,
	Struct,
	Event,
	Operator,
	TypeParameter,
	Comment
} token_t;

typedef struct token token;
struct token {
	char* str;
	token_t kind;
	int l, r, line_l, line_r;
};

typedef struct gwparser gwparser;
struct gwparser {
	wtree* keywords;
	vector* (* make_tokens)(gwparser*, char*);
	vector* (* make_completions)(gwparser*, char*, int, int);
	vector* (* validate)(gwparser*, char*);
};

gwparser* init_parser();
void free_parser(gwparser* self);

#endif //GW_BASIC_SERVER_PARSER_H
