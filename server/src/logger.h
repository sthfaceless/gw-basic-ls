//
// Created by danil on 25.04.2021.
//

#ifndef GW_BASIC_SERVER_LOGGER_H
#define GW_BASIC_SERVER_LOGGER_H

#include "stdlib.h"
#include "stdio.h"
#include "types.h"

typedef struct logger logger;

typedef enum {
    log_info,
    log_error,
    log_response,
    log_request,
    log_debug
} log_type;

struct logger {
    const char *filename;

    void (*log)(const log_type, const char *);
};

logger *Logger;

void initialize_logger(const char *filename);
void free_logger();

#endif //GW_BASIC_SERVER_LOGGER_H
