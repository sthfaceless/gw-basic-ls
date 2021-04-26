//
// Created by danil on 25.04.2021.
//

#ifndef GW_BASIC_SERVER_CSTRING_H
#define GW_BASIC_SERVER_CSTRING_H

#include <glob.h>
#include <stdlib.h>

#define DEFAULT_BUFF_SIZE 16

typedef struct string string;

struct string{
    char * str;
    size_t str_sz, buff_sz;
};

string * get_string();

#endif //GW_BASIC_SERVER_CSTRING_H
