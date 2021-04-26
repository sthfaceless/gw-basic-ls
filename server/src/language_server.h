//
// Created by danil on 25.04.2021.
//

#include "string.h"

#include "json.h"
#include "json-builder.h"
#include "logger.h"
#include "message.h"

#ifndef GW_BASIC_SERVER_LANGUAGE_SERVER_H
#define GW_BASIC_SERVER_LANGUAGE_SERVER_H

typedef struct language_server language_server;

struct language_server{
    _Bool (*process)(const language_server *, const char *, const size_t);
};

extern logger* Logger;

language_server * create_language_server();
void free_language_server(language_server * self);

#endif //GW_BASIC_SERVER_LANGUAGE_SERVER_H
