//
// Created by danil on 25.04.2021.
//

int debug_size_remove = 4;
char * debug_end_line = "\r\n\r\n";


#include "message.h"

const char *_get_message(const message *self) {
    return self->str;
}

void process_header(message *self) {
#ifdef MY_DEBUG
    debug_size_remove = 2;
    debug_end_line = "\n\n";
#endif
    if (self->size > 4 && strcmp(self->str + self->size - debug_size_remove, debug_end_line) == 0) {
        self->header_finished = 1;
        self->message_size = atoi(self->str + strlen("Content-Length:"));
        self->size = 0;
        self->str[0] = '\0';

        Logger->log(log_info, "Header processed");
    }
}

_Bool _is_finished(const message *self) {
    return self->header_finished && (self->size >= self->message_size);
}

void _add_char(message *self, const char c) {
    self->str[self->size++] = c;
    self->str[self->size] = '\0';
    if (!self->header_finished)
        process_header(self);
}

void _clear_message(message *self) {
    self->size = 0;
    self->message_size = 0;
    self->header_finished = 0;
    self->str[self->size] = '\0';
}

message *create_message() {
    message *msg = malloc(sizeof (message));
    msg->str = malloc(MAX_LSP_MESSAGE_SIZE);
    msg->size = 0;
    msg->message_size = 0;
    msg->header_finished = 0;

    msg->get_message = _get_message;
    msg->add_char = _add_char;
    msg->clear_message = _clear_message;
    msg->is_finished = _is_finished;
    return msg;
}

void free_message(message * self){
    free(self->str);
    free(self);
}