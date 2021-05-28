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
#include "tokenizer.h"

typedef struct gwparser gwparser;
struct gwparser {
	tokenizer* _tokenizer;
	vector* (* make_completions)(gwparser*, char*, int, int);
	vector* (* validate)(gwparser*, char*);
};
typedef struct completionItem completionItem;
struct completionItem{
	char *name;
	token_t kind;
};

gwparser* init_parser(json_value* config);
void free_completion_item(completionItem* item);
void free_completion_items(vector* items);
void free_parser(gwparser* self);

#endif //GW_BASIC_SERVER_PARSER_H
