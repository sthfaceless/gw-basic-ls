//
// Created by danil on 01.05.2021.
//

#include "parser.h"

static void init_if_null(int* dest, void* src) {
	if (!(*dest) && src)
		*dest = *(int*)src;
}

static token_t get_token_type(wtree* words, char* str) {
	int res = 0;
	if (matchb(str, "^[a-z]+$")) {
		init_if_null(&res, words->find(words, str));
	}
	return res;
}

static token* get_token(wtree* words, char* str, int l, int r, int line) {
	token* t = malloc(sizeof(token));
	t->str = str;
	t->l = l;
	t->r = r;
	t->line = line;
	t->kind = get_token_type(words, str);
	return t;
}

static vector* make_tokens(gwparser* self, char* str) {

	vector* vect = create_vector();
	int line_index = 0, l = 0, sz = 0, line = 0;
	char curr_token[1024];
	while (*str) {
		if (isspace(*str)) {
			if (sz) {
				curr_token[sz] = 0;
				vect->push(vect, get_token(self->keywords, copystr(curr_token), l, l+sz-1, line));
				sz = 0;
			}
			if (*(str+1) && *str=='\r' && *(str+1)=='\n') {
				str++, line++, line_index = 0;
			}
		}
		else if (isprint(*str)) {
			if (!sz) l = line_index;
			curr_token[sz++] = *str;
		}
		str++, line_index++;
	}

	return vect;
}

static vector* completion_bfs(wtree* tree, char* str) {

	int root = tree->find_ind(tree, str);
	if (root==-1)
		return create_vector_reserved(0);

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
			path->reverse(path);
			vect->push(vect, path->get_chars(path));
		}
		lvector* edges = tree->edges[vertex];
		iterator* it = edges->iterator(edges);
		while (it->has_next(it))
			queue->push(queue, (int*)it->get_next(it));
	}
	free_lvector_no_values(queue);

	return vect;
}

static vector* make_completions(gwparser* self, char* str, int line, int pos) {

	vector* lines = get_lines(str);
	vector* completions = NULL;
	void* lineptr = lines->get(lines, line);

	if (lineptr && pos<strlen((char*)lineptr) && !isspace(((char*)lineptr)[pos])) {

		char* strline = (char*)lineptr, item[1024];
		int sz = 0;
		while (pos>=0 && !isspace(strline[pos]))
			item[sz++] = strline[pos--];
		item[sz] = 0;

		completions = completion_bfs(self->keywords, item);
	}
	free_vector(lines);

	return completions ? completions : create_vector_reserved(0);
}

static wtree* load_keywords() {

	wtree* words = create_wtree();

	/*
	 * Functions
	 * */
	words->add(words, "print", wrapi(Function));

	return words;
}

gwparser* init_parser() {
	gwparser* self = malloc(sizeof(gwparser));

	self->keywords = load_keywords();
	self->make_tokens = make_tokens;
	self->make_completions = make_completions;

	return self;
}

void free_parser(gwparser* self) {
	free_wtree(self->keywords);
	free(self);
}

