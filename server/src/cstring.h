//
// Created by danil on 25.04.2021.
//

#ifndef GW_BASIC_SERVER_CSTRING_H
#define GW_BASIC_SERVER_CSTRING_H

#include <glob.h>
#include <stdlib.h>
#include "string.h"
#include <pcre.h>
#include "vector.h"
#include "map.h"
#include "ctype.h"

#define DEFAULT_BUFF_SIZE 16
#define MAX_FILE_SIZE 1024
#define WORD_TREE_SIZE 1 << 12
#define WORDS_BEGIN_SYMBOL 32
#define WORDS_END_SYMBOL 127

typedef struct string string;

struct string {
    int size, _chars_changed;
    vector *chars;
    char * _chars;
    int (*match)(string *, string *);
    void (*add)(string *, char);
    int (*find)(string *, string*);
    string* (*conc)(string*, string*);
    void (*set_at)(string *, int, char);
    char* (*get_chars)(string *);
    char (*char_at)(string *, int);
	void (*reverse)(string*);
};
string *get_string();
string *get_string_from(char *str);
void free_string(string* self);

typedef struct wtree wtree;
struct wtree{
	int ** tree, *parent;
	char * chars;
	void* *terminate;
	lvector* *edges;
	size_t size;
	void (*add)(wtree*, const char*, void*);
	void* (*find)(wtree*,const char*);
	int (*find_ind)(wtree*, const char*);
};

wtree* create_wtree();
void free_wtree(wtree* self);

vector* match(const char *str, char *pattern);
int matchb(const char *str, char *pattern);

void remove_spaces(char *str);

char* read_file(const char* filename);

vector * get_lines(char *str);
int is_eol(char ch);
void strlower(char *str);
char * copystr(char * str);

#endif //GW_BASIC_SERVER_CSTRING_H
