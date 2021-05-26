//
// Created by danil on 01.05.2021.
//

#include "parser.h"

static token_t get_token_type(wtree* words, char* str) {
	int res = 0;

//	Tree keyword
	if (matchb(str, "^[a-z]+$")) {
		res = *(int*)words->find(words, str);
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

static token* create_token(char* str, int l, int r, int line_l, int line_r) {
	token* t = malloc(sizeof(token));
	t->str = str;
	t->l = l;
	t->r = r;
	t->line_l = line_l;
	t->line_r = line_r;
	t->kind = Unknown;
	return t;
}

static token* get_token(wtree* words, char* str, int l, int r, int line_l, int line_r) {
	token* t = create_token(str, l, r, line_l, line_r);
	t->kind = get_token_type(words, str);
	return t;
}

void end_token(vector* vect, wtree* keywords, char* token, int* sz, int l, int line) {
	if (*sz) {
		token[*sz] = 0;
		vect->push(vect, get_token(keywords, copystr(token), l, l+(*sz)-1, line, line));
		*sz = 0;
	}
}

static vector* make_tokens(gwparser* self, char* str) {

	vector* vect = create_vector();
	int line_index = 0, l = 0, sz = 0, line = 0;
	char curr_token[1024];
	while (*str) {
		if (isspace(*str) || *str=='\"' || *str=='\'') {
			end_token(vect, self->keywords, curr_token, &sz, l, line);
			l = line_index;
			if (*str=='\"') {
				curr_token[sz++] = *(str++);
				while (*str && *str!='\"')
					curr_token[sz++] = *(str++);
				end_token(vect, self->keywords, curr_token, &sz, l, line);
			}
			else if (*str=='\'' || *str=='\n' || (*(str+1) && *str=='\r' && *(str+1)=='\n')) {

				while (*str && !(*str=='\n' || (*(str+1) && *str=='\r' && *(str+1)=='\n')))
					curr_token[sz++] = *(str++);

				end_token(vect, self->keywords, curr_token, &sz, l, line);
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
	vect->push(vect, get_token(self->keywords, copystr(curr_token), l, l+sz-1, line, line));

	return vect;
}

static vector* completion_bfs(wtree* tree, char* str) {

	int root = tree->find_ind(tree, str);
	if (root==-1)
		return create_vector_reserved(0);

	int strend = strlen(str)-1;

	lvector* queue = create_lvector();
	vector* vect = create_vector();
	queue->push(queue, &root);
	while (queue->size) {
		int vertex = *(int*)queue->get_last(queue);
		queue->pop(queue);
		if (vertex!=root && tree->terminate[vertex]) {
			int _vertex = vertex;
			string* path = get_string();
			while (_vertex!=root)
				path->add(path, tree->chars[_vertex]),
						_vertex = tree->parent[_vertex];

			for (char* c = str+strend; c>=str; c--)
				path->add(path, *c);
			path->reverse(path);

			token* t = create_token(path->get_chars(path), 0, 0, 0, 0);
			t->kind = *(int*)tree->terminate[vertex];
			vect->push(vect, t);
		}
		lvector* edges = tree->edges[vertex];
		iterator* it = edges->iterator(edges);
		while (it->has_next(it))
			queue->push(queue, (int*)it->get_next(it));
	}
	free_lvector(queue);

	return vect;
}

static vector* make_completions(gwparser* self, char* str, int line, int pos) {

	vector* completions = NULL;

	vector* lines = get_lines(str);
	void* lineptr = lines->get(lines, line);

	if (lineptr) {
		/*
		 * pos-1 указывает на конец слова, которое хотим продлить
		 * */
		pos = pos-1;
		if (pos>=0) {
			if (pos<strlen((char*)lineptr) && !isspace(((char*)lineptr)[pos])) {

				char* strline = (char*)lineptr, item[1024];
				int sz = 0;
				while (pos>=0 && !isspace(strline[pos]))
					item[sz++] = strline[pos--];
				reverse_charr(item, sz);
				item[sz] = 0;

				completions = completion_bfs(self->keywords, item);
			}
		}
		else {
			int line_index = 10;
			line = line-1;
			while (line>=0) {
				vector* tokens = make_tokens(self, lines->get(lines, line));
				if (tokens->size && ((token*)tokens->get(tokens, 0))->kind==Number) {
					line_index = (atoi(((token*)tokens->get(tokens, 0))->str)/10+1)*10;
					break;
				}
				line--;
			}

			char index_str[16];
			sprintf(index_str, "%d", line_index);
			token* tok = create_token(index_str, 0, 0, 0, 0);
			tok->kind = Number;

			completions = create_vector_from(1, tok);
		}
	}
	free_vector(lines);

	return completions ? completions : create_vector_reserved(0);
}

vector* make_tokens_with_lines(gwparser* self, char* text) {
	vector* lines = create_vector();
	vector* strlines = get_lines(text);
	iterator* it = strlines->iterator(lines);
	while (it->has_next(it)) {
		char* line = (char*)it->get_next(it);
		lines->push(lines, make_tokens(self, line));
	}
	return lines;
}

vector* validate(gwparser* self, char* text) {

	vector* messages = create_vector();

	vector* tokens = make_tokens_with_lines(self, text);
	for (int i = 0; i<tokens->size; ++i) {

		int last_index = ((token*)tokens->last(tokens))->r;
		if (last_index>255) {
			messages->push(messages, too_long_line(i, last_index));
		}

		if (tokens->size>0 && ((token*)tokens->get(tokens, 0))->kind!=Number) {
			messages->push(messages, must_begin_number(i, last_index));
		}

	}

	return messages;
}

static wtree* load_keywords() {

	wtree* words = create_wtree();

	/*
	 * Functions
	 * */
	words->add(words, "print", wrapi(Function));
	words->add(words, "play", wrapi(Function));

	return words;
}

gwparser* init_parser() {
	gwparser* self = malloc(sizeof(gwparser));

	self->keywords = load_keywords();
	self->make_tokens = make_tokens;
	self->make_completions = make_completions;
	self->validate = validate;

	return self;
}

void free_parser(gwparser* self) {
	free_wtree(self->keywords);
	free(self);
}

