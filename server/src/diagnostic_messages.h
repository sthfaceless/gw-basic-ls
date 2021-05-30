//
// Created by danil on 26.05.2021.
//

#ifndef GW_BASIC_SERVER_DIAGNOSTIC_MESSAGES_H
#define GW_BASIC_SERVER_DIAGNOSTIC_MESSAGES_H

#include "stdlib.h"
#include "vector.h"

typedef enum {
	Error,
	Warning,
	Information,
	Hint
} diagnostic_t;

typedef struct diagnostic diagnostic;
struct diagnostic {
	int line, l, r;
	diagnostic_t t;
	char* message;
};

diagnostic* too_long_line(int line, int last_character);
diagnostic* must_begin_number(int line, int last_character);
diagnostic* begin_number_too_large(int line, int l, int r);
void free_diagnostic_item(diagnostic* _diagnostic);
void free_diagnostic_items(vector* items);

#endif //GW_BASIC_SERVER_DIAGNOSTIC_MESSAGES_H
