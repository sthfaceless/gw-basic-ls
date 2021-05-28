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

typedef enum {
	Namespace,
	Type,
	Class,
	Enum,
	Interface,
	Struct,
	TypeParameter,
	Parameter,
	Variable,
	Property,
	EnumMember,
	Event,
	Function,
	Method,
	Macro,
	Keyword,
	Modifier,
	Comment,
	String,
	Number,
	Regexp,
	Operator,
	Unknown,
	ArithmeticOperator,
	SpaceDelimiter,
	NewlineDelimiter,
	CommentBegin,
	LogicalOperator,
	StatementDelimiter,
	ArgumentDelimiter,
	ArgumentGroupDelimiter,
	EqualDelimiter,
	IODelimiter,
	StringDelimiter
} token_t;


typedef struct range range;
struct range {
	int l, r, line_l, line_r;
};

typedef struct token token;
struct token {
	char* str;
	token_t kind;
	int l, r, line_l, line_r;
};

typedef enum {
	Begin,
	End
} TokenizerState;

typedef struct tokenizer tokenizer;
struct tokenizer {
	wtree *keywords;
	vector *__tokens;
	int __curr_index, __curr_line, __curr_char;
	char *__data, *__token;
	token_t *delimiters;
	wtree *delimiter_words;
	TokenizerState state;
	vector* (* make_tokens)(tokenizer*, char*);
	vector* (* make_tokens_with_range)(tokenizer*, char*, range*);
	vector* (* make_tokens_with_lines)(tokenizer*, char*);
};
typedef struct gwkeyword gwkeyword;
struct gwkeyword{
	char *name, *type, *purpose, *syntax;
	token_t kind;
};

token* find_token(vector* tokens_line, int pos);
range *create_range_object(int line_l, int l, int line_r, int r);
vector* get_token_types();
int map_type_to_semantic_token(token_t type);
token* create_token(char* str, int l, int r, int line_l, int line_r);
tokenizer* create_tokenizer(json_value* config);
void free_token_item(token* t);
void free_token_items(vector* t);
void free_tokens(vector* tokens);
void free_tokenizer(tokenizer* t);

#endif //GW_BASIC_SERVER_TOKENIZER_H
