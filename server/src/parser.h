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


typedef enum{
	CompletionUnknown,//0
	CompletionText,//1
	CompletionMethod,//2
	CompletionFunction,//3
	CompletionConstructor, //4
	CompletionField,// 5
	CompletionVariable,// 6
	CompletionClass, // 7
	CompletionInterface, //8
	CompletionModule,// 9
	CompletionProperty, //10
	CompletionUnit,// 11
	CompletionValue,// 12
	CompletionEnum,// 13
	CompletionKeyword, //14
	CompletionSnippet,//  15
	CompletionColor,//  16
	CompletionFile,//  17
	CompletionReference,//  18
	CompletionFolder,//  19
	CompletionEnumMember,//  20
	CompletionConstant,//  21
	CompletionStruct,//  22
	CompletionEvent,//  23
	CompletionOperator,//  24
	CompletionTypeParameter,//  25
} completion_t;

typedef struct completionItem completionItem;
struct completionItem{
	char *name, *detail;
	completion_t kind;
	gwkeyword* keyword;
};

typedef struct hoverItem hoverItem;
struct hoverItem{
	range *_range;
	gwkeyword* keyword;
};


typedef struct gwparser gwparser;
struct gwparser {
	tokenizer* _tokenizer;
	vector* (* make_completions)(gwparser*, document*, int, int);
	vector* (* validate)(gwparser*, document*);
	hoverItem * (*get_hover_item)(gwparser*, document*, int, int);
	range* (*get_declaration)(gwparser*, document*, int, int);
};
completion_t map_token_type_to_completion(token_t type);
gwparser* init_parser(json_value* config);
hoverItem* create_hover_item(gwkeyword* keyword, range* _range);
void free_hover_item(hoverItem *self);
void free_completion_item(completionItem* item);
void free_completion_items(vector* items);
void free_parser(gwparser* self);

#endif //GW_BASIC_SERVER_PARSER_H
