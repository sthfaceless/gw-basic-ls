//
// Created by danil on 25.04.2021.
//

#ifndef GW_BASIC_SERVER_CSTRING_H
#define GW_BASIC_SERVER_CSTRING_H

#include <glob.h>
#include <stdlib.h>

#define DEFAULT_BUFF_SIZE 16

typedef struct string string;

struct string {
    char *str;
    size_t str_sz, buff_sz;
};

string *get_string();

typedef struct {
    int **matches;
    size_t count;
} match_result;

match_result *match(const char *str, const char *pattern);

char ** split(const char *str, const char *sym);

int find_first(const char *str, const char *substr);

int contains(const char *str, const char *substr);

void remove_spaces(char *str);

#endif //GW_BASIC_SERVER_CSTRING_H
