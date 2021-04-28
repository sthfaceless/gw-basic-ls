//
// Created by danil on 27.04.2021.
//
#include "types.h"

static int int_comp(const void* arg1, const void* arg2) {
	ll res = (*(ll*)arg1 - *(ll*)arg2);
	if(res < 0) return -1;
	else if(res > 0) return 1;
	else return 0;
}

static int double_comp(const void* arg1, const void* arg2){
	ld res = (*(ld*)arg1 - *(ld*) arg2);
	if(res < 0) return -1;
	else if(res > 0) return 1;
	else return 0;
}

static int ch_comp(const void* arg1, const void *arg2){
	return (*(char*) arg1 - *(char *) arg2);
}

static int str_comp(const void* arg1, const void* arg2){
	return strcmp((char *) arg1, (char *) arg2);
}

static int ptr_comp(const void* arg1, const void* arg2){
	return int_comp(&arg1, &arg2);
}

comp get_comparator(type_t type) {
	switch (type) {
		case INT:
			return int_comp;
		case DOUBLE:
			return double_comp;
		case CHAR:
			return ch_comp;
		case STRING:
			return str_comp;
		case STRUCT:
			return ptr_comp;
		default:
			return ptr_comp;
	}
}

void swap(void* a, void* b){
	void* tmp = a;
	a = b;
	b = a;
}