//
// Created by danil on 25.04.2021.
//

#include "cstring.h"

static void update_chars(string* self) {
	if (self->_chars_changed) {
		self->_chars = realloc(self->_chars, sizeof(char)*(self->size+1));
		char* curr = self->_chars;
		void** item = self->chars->items;
		for (int i = 0; i<self->size; i++)
			*(curr++) = *(char*)*(item++);
		*curr = 0;
		self->_chars_changed = 0;
	}
}

static int smatch(string* self, string* pat) {
	return matchb(self->get_chars(self), pat->get_chars(pat));
}

static void add(string* self, char ch) {
	self->chars->push(self->chars, wrapc(ch));
	self->_chars_changed = 1;
	self->size++;
}

static string* conc(string* self, string* other) {
	self->chars->merge(self->chars, other->chars);
	self->size = self->chars->size;
	self->_chars_changed = 1;
	return self;
}
static void set_at(string* self, int pos, char ch) {
	self->chars->set(self->chars, pos, wrapc(ch));
	self->_chars_changed = 1;
}

static char char_at(string* self, int pos) {
	void* item = self->chars->get(self->chars, pos);
	if (item)
		return *(char*)item;
	return 0;
}

static char* get_chars(string* self) {
	update_chars(self);
	return self->_chars;
}

char* get_chars_and_terminate(string* self) {
	char* res = get_chars(self);
	free_vector(self->chars);
	free(self);
	return res;
}

static int find(string* self, string* oth) {
	return strstr(self->get_chars(self), oth->get_chars(oth))-self->get_chars(self);
}

static void reverse(string* self) {
	self->chars->reverse(self->chars);
}

static string* init_string() {

	string* self = malloc(sizeof(string));
	self->size = 0;
	self->_chars_changed = 1;
	self->_chars = malloc(sizeof(char));

	self->get_chars = get_chars;
	self->get_chars_and_terminate = get_chars_and_terminate;
	self->char_at = char_at;
	self->set_at = set_at;
	self->conc = conc;
	self->find = find;
	self->add = add;
	self->reverse = reverse;

	return self;
}

string* get_string() {

	string* self = init_string();
	self->chars = create_vector_reserved(DEFAULT_BUFF_SIZE);

	return self;
}
string* get_string_from(char* str) {

	string* self = init_string();
	self->size = strlen(str);
	self->chars = create_vector_sized(self->size, NULL);

	void** item = self->chars->items;
	while (*str)
		*(item++) = str++;

	return self;
}

void free_string(string* self) {
	free_vector(self->chars);
	free(self->_chars);
	free(self);
}

void remove_spaces(char* s) {
	const char* d = s;
	do {
		while (*d==' ') {
			++d;
		}
	}
	while (*s++ = *d++);
}

vector* match(const char* str, char* pattern) {

	static map* pattern_cache = NULL;
	if (pattern_cache==NULL)
		pattern_cache = create_map(STRING);

	void* req_ptr = pattern_cache->get(pattern_cache, pattern);
	if (req_ptr==NULL) {
		const char* error;
		int error_offset;
		req_ptr = pcre_compile(pattern, 0, &error, &error_offset, NULL);
		pattern_cache->put(pattern_cache, pattern, req_ptr);
	}
	pcre* req_re = (pcre*)req_ptr;

	vector* vect = create_vector_reserved(2);
	if (req_re!=NULL) {
		int match_vector[1026];
		int rc = pcre_exec(req_re, NULL, str, strlen(str), 0, 0,
				match_vector, sizeof(match_vector)/sizeof(match_vector[0]));
		if (rc>=0) {
			for (int i = 0; i<rc; ++i) {
				int* pair = malloc(2*sizeof(int));
				pair[0] = match_vector[2*i];
				pair[1] = match_vector[2*i+1]-1;
				vect->push(vect, pair);
			}
		}
	}

	return vect;
}

int matchb(const char* str, char* pattern) {
	return match(str, pattern)->size;
}

char* read_file(const char* filename) {

	FILE* fptr = fopen(filename, "a");
	char* res = malloc(sizeof(char)*MAX_FILE_SIZE),
			* curr = res;
	int ch;
	while ((ch = fgetc(fptr))!=-1) {
		*(curr++) = ch;
	}
	*curr = 0;

	fclose(fptr);

	return res;
}

int is_eol(char ch) {
	return ch=='\r' || ch=='\n';
}

void strlower(char* str) {
	while (*str) {
		*str = tolower(*str);
		str++;
	}
}
char* copystr(const char* str) {
	if (str==NULL) {
		char* emp = malloc(sizeof(char));
		emp[0] = 0;
		return emp;
	}
	return strdup(str);
}

vector* get_lines(char* str) {
	vector* vect = create_vector();
	string* temp_str = get_string();
	while (*str) {
		if (isprint(*str))
			temp_str->add(temp_str, *str);
		if (!*(str+1)
				|| (*str=='\r' && *(str+1)=='\n')
				|| *str=='\n') {
			vect->push(vect, copystr(temp_str->get_chars(temp_str)));
			free_string(temp_str);
			if (*(str+1))
				temp_str = get_string();
		}
		str++;
	}
	return vect;
}
char* substr(char* str, int begin, int end) {
	char* res = malloc(sizeof(char)*(end-begin+1+1));
	strncpy(res, str, end-begin+1);
	res[end-begin+1] = 0;
	return res;
}
vector* strsplit(char* str, char tok) {
	vector* tokens = create_vector();
	char* begin = str;
	while (*str) {
		if (*str==tok) {
			if (str-begin)
				tokens->push(tokens, substr(begin, 0, str-begin-1));
			begin = str+1;
		}
		str++;
	}
	if (str-begin)
		tokens->push(tokens, substr(begin, 0, str-begin-1));
	return tokens;
}

static int create_wtree_node(wtree* self, void* payload, char ch, int parent) {
	const int MAX_SYMBOLS = WORDS_END_SYMBOL-WORDS_BEGIN_SYMBOL+1;
	int* node = malloc(sizeof(int)*MAX_SYMBOLS);
	memset(node, 0, MAX_SYMBOLS*sizeof(int));
	self->tree[self->size] = node;
	self->parent[self->size] = parent;
	self->chars[self->size] = ch;
	self->terminate[self->size] = payload;
	self->edges[self->size] = create_lvector();
	self->size++;
	return self->size-1;
}

static void add_wtree(wtree* self, const char* str, void* payload) {
	int** curr = self->tree;
	while (isprint(*str)) {
		char ch = *str-WORDS_BEGIN_SYMBOL;
		if ((*curr)[ch]==-1) {
			int node_id = create_wtree_node(self, NULL, *str, curr-self->tree);
			(*curr)[ch] = node_id;
			self->edges[curr-self->tree]->push(self->edges[curr-self->tree], wrapi(node_id));
		}
		curr = self->tree+(*curr)[ch];
		str++;
	}
	if (curr!=self->tree)
		self->terminate[curr-self->tree] = payload;
}
static int find_ind(wtree* self, const char* str) {
	int** curr = self->tree;
	while (isprint(*str)) {
		char ch = *str-WORDS_BEGIN_SYMBOL;
		if ((*curr)[ch]==-1)
			return -1;
		curr = self->tree+(*curr)[ch];
		str++;
	}
	return curr-self->tree;
}

static void* find_wtree(wtree* self, const char* str) {
	int ind = find_ind(self, str);
	if (ind!=-1)
		return self->terminate[ind];
	return NULL;
}

static void* walk(wtree* self, const char* str) {
	int curr = 0;
	while (*str && (curr = self->tree[curr][*str]));
	return self->terminate[curr];
}

void* walk_path(wtree* self, const char* str) {
	int curr = 0;
	string* path = get_string();
	while (*str && (curr = self->tree[curr][*str]))
		path->add(path, self->chars[curr]);
	return path->get_chars_and_terminate(path);
}

wtree* create_wtree_sized(size_t size) {
	wtree* self = malloc(sizeof(wtree));
	self->tree = malloc(sizeof(int*)*size);
	self->parent = malloc(sizeof(int)*size);
	self->chars = malloc(sizeof(char)*size);
	self->terminate = malloc(sizeof(void*)*size);
	self->edges = malloc(sizeof(lvector*)*size);
	self->size = 0;
	create_wtree_node(self, NULL, 0, -1);
	self->add = add_wtree;
	self->find = find_wtree;
	self->find_ind = find_ind;
	self->walk = walk;
	self->walk_path = walk_path;
	return self;
}

wtree* create_wtree() {
	return create_wtree_sized(WORD_TREE_SIZE);
}
void free_wtree(wtree* self) {
	for (int i = 0; i<self->size; ++i) {
		free(self->tree[i]);
		free(self->terminate[i]);
		free_lvector(self->edges[i]);
	}
	free(self->tree);
	free(self->terminate);
	free(self->edges);
	free(self);
}