//
// Created by danil on 27.04.2021.
//
#include "types.h"

static int int_comp(const void* arg1, const void* arg2) {
	if(!arg1 || !arg2) return 1;

	ll res = (*(ll*)arg1 - *(ll*)arg2);
	if(res < 0) return -1;
	else if(res > 0) return 1;
	else return 0;
}

static int double_comp(const void* arg1, const void* arg2){
	if(!arg1 || !arg2) return 1;

	ld res = (*(ld*)arg1 - *(ld*) arg2);
	if(res < 0) return -1;
	else if(res > 0) return 1;
	else return 0;
}

static int ch_comp(const void* arg1, const void *arg2){
	if(!arg1 || !arg2) return 1;
	return (*(char*) arg1 - *(char *) arg2);
}

static int str_comp(const void* arg1, const void* arg2){
	if(!arg1 || !arg2) return 1;
	return strcmp((char *) arg1, (char *) arg2);
}

static int ptr_comp(const void* arg1, const void* arg2){
	if(!arg1 || !arg2) return 1;
	return int_comp(&arg1, &arg2);
}

comp get_comparator(type_t type) {
	switch (type) {
		case INT_KEY:
			return int_comp;
		case DOUBLE_KEY:
			return double_comp;
		case CHAR_KEY:
			return ch_comp;
		case STRING_KEY:
			return str_comp;
		case STRUCT_KEY:
			return ptr_comp;
		default:
			return ptr_comp;
	}
}

int* wrapi(int val){
	int *val_wrapper = malloc(sizeof (int));
	*val_wrapper = val;
	return val_wrapper;
}

char* wrapc(char val){
	char *char_wrapper = malloc(sizeof (char) * 2);
	*char_wrapper = val;
	*(char_wrapper+1) = 0;
	return char_wrapper;
}

void swap(void* *a, void* *b){
	void* tmp = *a;
	*a = *b;
	*b = tmp;
}

void reverse_charr(char* arr, int sz){
	for(char* l = arr, *r = arr+sz-1; l < r; l++, r--) {
		ll tmp = *l;
		*l = *r;
		*r = tmp;
	}
}

ll getmax(ll val1, ll val2){
	return val1 > val2 ? val1 : val2;
}

ll getmin(ll val1, ll val2){
	return val1 < val2 ? val1 : val2;
}
