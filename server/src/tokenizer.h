//
// Created by danil on 28.05.2021.
//

#ifndef GW_BASIC_SERVER_TOKENIZER_H
#define GW_BASIC_SERVER_TOKENIZER_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "cstring.h"
#include "logger.h"
#include "map.h"
#include "json.h"
#include "json-builder.h"
#include "json_helper.h"

#define MAX_SYMBOLS 256

typedef enum
{
	Namespace,//0
	Type,//1
	Class,//2
	Enum,//3
	Interface,//4
	Struct,//5
	TypeParameter,//6
	Parameter,//7
	Variable,//8
	Property,//9
	EnumMember,//10
	Event,//11
	Function,//12
	Method,//13
	Macro,//14
	Keyword,//15
	Modifier,//16
	Comment,//17
	String,//18
	Number,//19
	Regexp,//20
	Operator,//21
	Unknown,//22
	ArithmeticOperator,//23
	LogicalOperator,//24
	SpaceDelimiter,//25
	NewlineDelimiter,//26
	CommentDelimiter,//27
	StatementDelimiter,//28
	ArgumentDelimiter,//29
	ArgumentGroupDelimiter,//30
	EqualDelimiter,//31
	IODelimiter,//32
	StringDelimiter,//33
	IntegerVariable,//34
	StringVariable,//35
	ArrayIntegerVariable,//36
	ArraySinglePrecisionVariable,//37
	ArrayDoublePrecisionVariable,//38
	ArrayStringVariable,//39
	SinglePrecisionVariable,//40
	DoublePrecisionVariable,//41
	IntegerValue,//42
	SinglePrecisionValue,//43
	DoublePrecisionValue,//44
	LineNumber,//45
	DimStatement//46
} token_t;

typedef struct range range;

struct range
{
	int l, r, line_l, line_r;
};

typedef struct token token;

struct token
{
	char* str;
	token_t kind;
	int l, r, line_l, line_r;
};

typedef enum
{
	Free,
	EndOfData,
	StringReading,
	CommentReading
} TokenizerState;

typedef struct tokenizer tokenizer;

struct tokenizer
{
	wtree* keywords;
	lvector* __tokens;
	int __curr_line, __curr_char, __token_len;
	char* __data, * __token;
	token_t* delimiters;
	wtree* delimiter_words;
	TokenizerState state;
	lvector*
	(* make_tokens)(tokenizer*, char*);
	lvector*
	(* make_tokens_with_range)(tokenizer*, char*, range*);
	vector*
	(* make_tokens_with_lines)(tokenizer*, char*);
};

typedef struct gwkeyword gwkeyword;

struct gwkeyword
{
	char* name, * type, * purpose, * syntax;
	token_t kind;
};

token* find_token(vector* tokens_line, int pos);
range* create_range_object(int line_l, int l, int line_r, int r);
vector* get_token_modifiers();
vector* get_token_types();
int map_type_to_semantic_token(token_t type);
token* create_token(const char* str, int l, int r, int line_l, int line_r);
tokenizer* create_tokenizer(json_value* config);
void free_token_item(token* t);
void free_token_items(lvector* t);
void free_tokens(vector* tokens);
void free_tokenizer(tokenizer* t);

#endif //GW_BASIC_SERVER_TOKENIZER_H
