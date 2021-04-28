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

#define DEFAULT_BUFF_SIZE 16

typedef struct string string;

struct string {
    char *str;
    size_t str_sz, buff_sz;
};

string *get_string();

vector* match(const char *str, char *pattern);

void remove_spaces(char *str);

#endif //GW_BASIC_SERVER_CSTRING_H
