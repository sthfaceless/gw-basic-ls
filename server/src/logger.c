//
// Created by danil on 25.04.2021.
//

#include "logger.h"


static void dolog(const log_type type, const char *str) {
    FILE *fptr = fopen(Logger->filename, "a");
    switch (type) {
        case log_info:
            fprintf(fptr, "[Info]: %s%s", str, LINE_SEPARATOR);
            break;
        case log_error:
            fprintf(fptr, "[Error]: %s%s", str,  LINE_SEPARATOR);
            break;
        case log_request:
            fprintf(fptr, "[Request]: %s%s", str, LINE_SEPARATOR);
            break;
        case log_response:
            fprintf(fptr, "[Response]: %s%s", str, LINE_SEPARATOR);
            break;
		case log_debug:
			fprintf(fptr, "[Debug]: %s%s", str, LINE_SEPARATOR);
			break;
        default:
            break;
    }
    fclose(fptr);
}

void initialize_logger(const char *filename) {
    Logger = malloc(sizeof(logger));
    Logger->log = dolog;
    Logger->filename = filename;
}

void free_logger() {
    free(Logger);
}