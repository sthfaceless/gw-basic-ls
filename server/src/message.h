//
// Created by danil on 25.04.2021.
//

#ifndef GW_BASIC_SERVER_MESSAGE_H
#define GW_BASIC_SERVER_MESSAGE_H

#include "stdlib.h"
#include "string.h"
#include "logger.h"

#define MAX_LSP_MESSAGE_SIZE sizeof(char) * ((1 << 16) + 1)

typedef struct message message;

struct message {
    char *str;
    size_t size, message_size;
    _Bool header_finished;

    void (*add_char)(message *, const char c);

    const char *(*get_message)(const message *);

    void (*clear_message)(message *);

    _Bool (*is_finished)(const message *);
};

extern logger* Logger;

message *create_message();
void free_message(message * self);

#endif //GW_BASIC_SERVER_MESSAGE_H
