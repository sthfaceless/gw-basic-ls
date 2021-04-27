//
// Created by danil on 25.04.2021.
//

#include "cstring.h"

string * get_string(){
    string *str = malloc(sizeof (string));

    str->buff_sz = DEFAULT_BUFF_SIZE;
    str->str = malloc(sizeof (char) * (DEFAULT_BUFF_SIZE + 1));
    str->str[0] = '\0';

    str->str_sz = 0;
    return str;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}