//
// Created by danil on 26.05.2021.
//

#include "diagnostic_messages.h"

diagnostic* create_diagnostic(){
	diagnostic *dgn = malloc(sizeof (diagnostic));
	return dgn;
}

diagnostic* too_long_line(int line, int last_character){
	diagnostic *dgn = create_diagnostic();
	dgn->l = 0;
	dgn->r = last_character;
	dgn->line = line;
	dgn->t = Error;
	dgn->message = "Длина строки не может превышать 256 символов";
	return dgn;
}

diagnostic* must_begin_number(int line, int last_character){
	diagnostic *dgn = create_diagnostic();
	dgn->l = 0;
	dgn->r = last_character;
	dgn->line = line;
	dgn->t = Error;
	dgn->message = "Строка обязана начинаться с цифры";
	return dgn;
}