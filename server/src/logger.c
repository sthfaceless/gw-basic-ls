//
// Created by danil on 25.04.2021.
//

#include "logger.h"


void _log(const log_type type, const char *str) {
    FILE *fptr = fopen(Logger->filename, "a");
    switch (type) {
        case log_info:
            fprintf(fptr, "[Info]: %s \r\n", str);
            break;
        case log_error:
            fprintf(fptr, "[Error]: %s\r\n", str);
            break;
        case log_request:
            fprintf(fptr, "[Request]: %s\r\n", str);
            break;
        case log_response:
            fprintf(fptr, "[Response]: %s\r\n", str);
            break;
        default:
            break;
    }
    fclose(fptr);
}

void initialize_logger(const char *filename) {
    Logger = malloc(sizeof(logger));
    Logger->log = _log;
    Logger->filename = filename;
}

void free_logger() {
    free(Logger);
}