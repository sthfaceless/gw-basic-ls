//
// Created by danil on 25.04.2021.
//

#include "message.h"

static const char *get_message(const message *self) {
    return self->str;
}

static int header_ended(const char *str, int size){
	int cnt = 0;
	while(*(str + size - 1) == '\n'){
		if(*(str+size-2) == '\r') size--;
		size--;
		cnt++;
	}
	return cnt == 2;
}

static void process_header(message *self) {
    if (self->size > 4 && header_ended(self->str, self->size)) {
        self->header_finished = 1;
        self->message_size = atoi(self->str + strlen("Content-Length:"));
        self->size = 0;
        self->str[0] = '\0';

        Logger->log(log_info, "Header processed");
    }
}

static _Bool is_finished(const message *self) {
    return self->header_finished && (self->size >= self->message_size);
}

static void add_char(message *self, const char c) {
    self->str[self->size++] = c;
    self->str[self->size] = '\0';
    if (!self->header_finished)
        process_header(self);
}

static void clear_message(message *self) {
    self->size = 0;
    self->message_size = 0;
    self->header_finished = 0;
    self->str[self->size] = '\0';
}

message *create_message() {
    message *msg = malloc(sizeof(message));
    msg->str = malloc(MAX_LSP_MESSAGE_SIZE);
    msg->size = 0;
    msg->message_size = 0;
    msg->header_finished = 0;

    msg->get_message = get_message;
    msg->add_char = add_char;
    msg->clear_message = clear_message;
    msg->is_finished = is_finished;
    return msg;
}

void free_message(message *self) {
    free(self->str);
    free(self);
}