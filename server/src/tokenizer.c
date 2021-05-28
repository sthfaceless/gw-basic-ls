//
// Created by danil on 28.05.2021.
//

#include "tokenizer.h"

vector* get_token_types() {
	vector* types = create_vector();
	types->push(types, "namespace");
	types->push(types, "type");
	types->push(types, "class");
	types->push(types, "enum");
	types->push(types, "interface");
	types->push(types, "struct");
	types->push(types, "typeParameter");
	types->push(types, "parameter");
	types->push(types, "variable");
	types->push(types, "property");
	types->push(types, "enumMember");
	types->push(types, "event");
	types->push(types, "function");
	types->push(types, "method");
	types->push(types, "macro");
	types->push(types, "keyword");
	types->push(types, "modifier");
	types->push(types, "comment");
	types->push(types, "string");
	types->push(types, "number");
	types->push(types, "regexp");
	types->push(types, "operator");
	return types;
}

int map_type_to_semantic_token(token_t type) {

	static token_t* mapper = NULL;
	if (mapper==NULL) {

		mapper = malloc(sizeof(int)*64);

		/*
		 * Default types
		 * */
		mapper[Namespace] = Namespace;
		mapper[Type] = Type;
		mapper[Enum] = Enum;
		mapper[Interface] = Interface;
		mapper[Struct] = Struct;
		mapper[TypeParameter] = TypeParameter;
		mapper[Parameter] = Parameter;
		mapper[Variable] = Variable;
		mapper[Property] = Property;
		mapper[EnumMember] = EnumMember;
		mapper[Event] = Event;
		mapper[Function] = Function;
		mapper[Method] = Method;
		mapper[Macro] = Macro;
		mapper[Keyword] = Keyword;
		mapper[Modifier] = Modifier;
		mapper[Comment] = Comment;
		mapper[String] = String;
		mapper[Number] = Number;
		mapper[Regexp] = Regexp;
		mapper[Operator] = Operator;
		mapper[Unknown] = Unknown;

		/*
		 * Extra types
		 * */
		mapper[ArithmeticOperator] = Operator;
		mapper[SpaceDelimiter] = Unknown;
		mapper[NewlineDelimiter] = Unknown;
		mapper[CommentBegin] = Modifier;
		mapper[LogicalOperator] = Operator;
		mapper[StatementDelimiter] = Modifier;
		mapper[ArgumentDelimiter] = Modifier;
		mapper[ArgumentGroupDelimiter] = Unknown;
		mapper[EqualDelimiter] = Operator;
		mapper[IODelimiter] = Modifier;
		mapper[StringDelimiter] = String;
	}

	return mapper[type];
}

static token_t get_token_type(wtree* words, char* str) {
	int res = 0;

	if (!strlen(str))
		return Unknown;

//	Tree keyword
	if (matchb(str, "^[a-z]+$")) {
		int* type = words->find(words, str);
		if (type) res = *type;
	}
//	Obvious data types
	if (res==0) {
		if (matchb(str, "^\\d+$")) {
			res = Number;
		}
		else if (matchb(str, "^\"(.|\\s)*\"$")) {
			res = String;
		}
		else if (matchb(str, "^\'.*")) {
			res = Comment;
		}
	}
	return res;
}

range* create_range_object(int line_l, int l, int line_r, int r) {
	range* rng = malloc(sizeof(range));
	rng->l = l;
	rng->r = r;
	rng->line_l = line_l;
	rng->line_r = line_r;
	return rng;
}

token* create_token(char* str, int l, int r, int line_l, int line_r) {
	token* t = malloc(sizeof(token));
	t->str = str;
	t->l = l;
	t->r = r;
	t->line_l = line_l;
	t->line_r = line_r;
	t->kind = Unknown;
	return t;
}

static token* get_token(tokenizer* self, char* str, int l, int r, int line_l, int line_r) {
	token* t = create_token(str, l, r, line_l, line_r);
	t->kind = get_token_type(self->keywords, str);
	return t;
}

static void end_token(vector* vect, tokenizer* self, char* token, int* sz, int l, int line) {
	if (*sz) {
		token[*sz] = 0;
		vect->push(vect, get_token(self, copystr(token), l, l+(*sz)-1, line, line));
		*sz = 0;
	}
}

static void finalize_token(tokenizer* self) {
	size_t len = strlen(self->__token);
	self->__tokens->push(self->__tokens,
			get_token(self, self->__token,
					self->__curr_char-len+1, self->__curr_char,
					self->__curr_line, self->__curr_line));
}

static vector* tokenize(tokenizer* self, char* str) {

	self->__tokens = create_vector();
	self->state = Begin;
	self->__data = str;
	self->__curr_index = 0, self->__curr_line = 0, self->__curr_char = 0;

	while (self->state!=End) {
		char character = self->__data[self->__curr_index];
		int* walk_result = NULL;
		if (!character) {
			finalize_token(self);
			self->state = End;
		}
		else if (self->delimiters[character] || (walk_result = self->delimiter_words->walk(self->delimiter_words, str))) {
			finalize_token(self);
			int delimiter_kind = self->delimiters[character] | *walk_result;
			if (!(delimiter_kind==SpaceDelimiter
				|| delimiter_kind==NewlineDelimiter
				|| delimiter_kind==StringDelimiter
				|| delimiter_kind==CommentBegin)) {
				char* name = self->delimiters[character] ? wrapc(character)
						: self->delimiter_words->walk_path(self->delimiter_words, str);
				token* _token = create_token(name, self->__curr_char, self->__curr_char+strlen(name)-1,
						self->__curr_line, self->__curr_line);
				_token->kind = delimiter_kind;
				self->__tokens->push(self->__tokens, _token);
			}
			else if (delimiter_kind==NewlineDelimiter) {
				//Todo \r\n take together
				self->__curr_line++, self->__curr_char = -1;
			}else if(delimiter_kind == StringDelimiter){
				//Todo take all string (mb with state?)
			}else if(delimiter_kind == CommentBegin){
				//Todo take all comment (mb with state?)
			}
		}
		self->__curr_index++, self->__curr_char++;
	}

	return self->__tokens;
}

static vector* make_tokens(tokenizer* self, char* str) {

	vector* vect = create_vector();
	int line_index = 0, l = 0, sz = 0, line = 0;
	char curr_token[1024];
	while (*str) {
		if (isspace(*str) || *str=='\"' || *str=='\'') {
			end_token(vect, self, curr_token, &sz, l, line);
			l = line_index;
			if (*str=='\"') {
				curr_token[sz++] = *(str++);
				while (*str && *str!='\"')
					curr_token[sz++] = *(str++);
				end_token(vect, self, curr_token, &sz, l, line);
			}
			else if (*str=='\'' || *str=='\n' || (*(str+1) && *str=='\r' && *(str+1)=='\n')) {

				while (*str && !(*str=='\n' || (*(str+1) && *str=='\r' && *(str+1)=='\n')))
					curr_token[sz++] = *(str++);

				end_token(vect, self, curr_token, &sz, l, line);
				line++, line_index = -1;
			}
		}
		else if (isprint(*str)) {
			if (!sz) l = line_index;
			curr_token[sz++] = *str;
		}
		str++, line_index++;
	}
	//last token
	curr_token[sz] = 0;
	vect->push(vect, get_token(self, copystr(curr_token), l, max(l+sz-1, 0), line, line));

	return vect;
}

static vector* make_tokens_with_range(tokenizer* self, char* text, range* rng) {

	vector* tokens = self->make_tokens(self, text);
	vector* result = create_vector();

	iterator* it = tokens->iterator(tokens);
	while (it->has_next(it)) {
		token* tkn = it->get_next(it);
		if (tkn->line_l>=rng->line_l && tkn->l>=rng->l && tkn->r<=rng->r && tkn->line_r<=rng->line_r)
			result->push(result, tkn);
	}
	free_vector_no_values(tokens);

	return result;
}

static vector* make_tokens_with_lines(tokenizer* self, char* text) {

	vector* lines = create_vector();
	int curr_line = -1;

	vector* tokens = self->make_tokens(self, text);
	iterator* it = tokens->iterator(tokens);

	while (it->has_next(it)) {
		token* tkn = it->get_next(it);
		while (curr_line<tkn->line_l)
			lines->push(lines, create_vector()),
					curr_line++;
		vector* last_line = lines->last(lines);
		last_line->push(last_line, tkn);
	}
	free_vector_no_values(tokens);

	return lines;
}

token* find_token(vector* tokens_line, int pos) {
	iterator* it = tokens_line->iterator(tokens_line);
	token* result_tkn = NULL;
	while (it->has_next(it)) {
		token* tkn = it->get_next(it);
		if (tkn->l<=pos)
			result_tkn = tkn;
		else
			break;
	}
	return result_tkn;
}

static gwkeyword*
get_gwkeyword(const char* name, const char* type, const char* purpose, const char* syntax, const token_t kind) {
	gwkeyword* keyword = malloc(sizeof(keyword));
	keyword->name = copystr(name);
	keyword->type = copystr(type);
	keyword->purpose = copystr(purpose);
	keyword->syntax = copystr(syntax);
	keyword->kind = kind;
	return keyword;
}

static wtree* load_keywords(json_value* config) {

	wtree* words = create_wtree();
	json_value* keywords = get_by_name(config, "keywords");
	for (int i = 0; i<keywords->u.array.length; ++i) {
		json_value* keyword = keywords->u.array.values[i];
		words->add(words,
				get_by_name(keyword, "name")->u.string.ptr,
				get_gwkeyword(
						get_by_name(keyword, "name")->u.string.ptr,
						get_by_name(keyword, "type")->u.string.ptr,
						get_by_name(keyword, "purpose")->u.string.ptr,
						get_by_name(keyword, "syntax")->u.string.ptr,
						get_by_name(keyword, "kind")->u.integer));
	}

	return words;
}

void fill_delimiters(tokenizer* t) {

	t->delimiters = malloc(sizeof(token_t)*MAX_SYMBOLS);
	memset(t->delimiters, 0, sizeof(token_t)*MAX_SYMBOLS);

	t->delimiter_words = create_wtree_sized(64);

	/*
	 * Arithmetic operators
	 * */
	t->delimiters['+'] = ArithmeticOperator;
	t->delimiters['-'] = ArithmeticOperator;
	t->delimiters['*'] = ArithmeticOperator;
	t->delimiters['\\'] = ArithmeticOperator;
	t->delimiters['/'] = ArithmeticOperator;
	t->delimiters['^'] = ArithmeticOperator;

	/*
	 * New line
	 * */
	t->delimiters['\n'] = NewlineDelimiter;
	t->delimiters['\r'] = NewlineDelimiter;

	/*
	 * Spaces
	 * */
	t->delimiters[' '] = SpaceDelimiter;
	t->delimiters['\t'] = SpaceDelimiter;

	/*
	 * Semantic delimiters
	 * */
	t->delimiters[':'] = StatementDelimiter;
	t->delimiters[','] = ArgumentDelimiter;
	t->delimiters['('] = ArgumentGroupDelimiter;
	t->delimiters[')'] = ArgumentGroupDelimiter;
	t->delimiters[';'] = IODelimiter;
	t->delimiters['\''] = CommentBegin;
	t->delimiters['\"'] = StringDelimiter;

	/*
	 * Logical delimiters
	 * */
	t->delimiters['='] = EqualDelimiter;
	t->delimiters['>'] = LogicalOperator;
	t->delimiters['<'] = LogicalOperator;
	t->delimiter_words->add(t->delimiter_words, "<>", wrapi(LogicalOperator));
	t->delimiter_words->add(t->delimiter_words, "<=", wrapi(LogicalOperator));
	t->delimiter_words->add(t->delimiter_words, ">=", wrapi(LogicalOperator));

}

tokenizer* create_tokenizer(json_value* config) {
	tokenizer* t = malloc(sizeof(tokenizer));
	t->keywords = load_keywords(config);
	fill_delimiters(t);
	t->make_tokens_with_lines = make_tokens_with_lines;
	t->make_tokens = make_tokens;
	t->make_tokens_with_range = make_tokens_with_range;
	return t;
}

void free_token_item(token* t) {
	free(t->str);
	free(t);
}

void free_token_items(vector* items) {

	for (int i = 0; i<items->size; ++i) {
		free_token_item(items->get(items, i));
	}
	free_vector_no_values(items);

}

void free_tokens(vector* tokens) {
	iterator* it = tokens->iterator(tokens);
	while (it->has_next(it))
		free_vector(it->get_next(it));
	free_vector_no_values(tokens);
}

void free_tokenizer(tokenizer* t) {
	free_wtree(t->keywords);
	free_wtree(t->delimiter_words);
	free(t->delimiters);
	free(t);
}
