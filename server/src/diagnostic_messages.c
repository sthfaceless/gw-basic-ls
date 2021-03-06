//
// Created by danil on 26.05.2021.
//

#include "diagnostic_messages.h"

diagnostic* create_diagnostic() {
	diagnostic* dgn = malloc(sizeof(diagnostic));
	return dgn;
}

void free_diagnostic_item(diagnostic* _diagnostic) {
	free(_diagnostic);
}
void free_diagnostic_items(vector* items) {
	for (int i = 0; i<items->size; ++i) {
		free_diagnostic_item(items->get(items, i));
	}
	free_vector_no_values(items);
}

diagnostic* too_long_line(int line, int last_character) {
	diagnostic* dgn = create_diagnostic();
	dgn->l = 0;
	dgn->r = last_character;
	dgn->line = line;
	dgn->t = Error;
	dgn->message = "Max line length is 256 characters";
	return dgn;
}

diagnostic* must_begin_number(int line, int last_character) {
	diagnostic* dgn = create_diagnostic();
	dgn->l = 0;
	dgn->r = last_character;
	dgn->line = line;
	dgn->t = Error;
	dgn->message = "Line must begin with number";
	return dgn;
}

diagnostic* begin_number_too_large(int line, int l, int r){
	diagnostic* dgn = create_diagnostic();
	dgn->l = l;
	dgn->r = r;
	dgn->line = line;
	dgn->t = Error;
	dgn->message = "Line number must be in range 0-65529";
	return dgn;
}