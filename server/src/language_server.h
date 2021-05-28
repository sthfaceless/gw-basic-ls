//
// Created by danil on 25.04.2021.
//


#ifndef GW_BASIC_SERVER_LANGUAGE_SERVER_H
#define GW_BASIC_SERVER_LANGUAGE_SERVER_H

#include <debug.h>

#include "string.h"
#include "json.h"
#include "json-builder.h"
#include "json_helper.h"
#include "logger.h"
#include "message.h"
#include "cstring.h"
#include "parser.h"

typedef struct language_server language_server;

struct language_server {
	gwparser* parser;
	map * documents;
	char *send_message_buf;
	json_value *config;
    _Bool (*process)(language_server *, const char *, const size_t);
};

extern logger *Logger;

language_server *create_language_server(char *config_file);

void free_language_server(language_server *self);

#endif //GW_BASIC_SERVER_LANGUAGE_SERVER_H
