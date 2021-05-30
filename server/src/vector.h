//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_VECTOR_H
#define GW_BASIC_SERVER_VECTOR_H

//#include <glob.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stdio.h"
#include "types.h"

#define DEFAULT_VECTOR_SIZE 16
#define VECTOR_EXPANSION_CONSTANT 2

typedef struct iterator iterator;
struct iterator {
	void* _iterable;
	int curr;
	int (*has_next)(iterator*);
	void* (* get_next)(iterator*);
};

typedef struct vector vector;
struct vector {
	void** items;
	size_t size, _size;

	void* (* get)(const vector*, const int);

	void* (* last)(const vector*);

	void (* push)(vector*, void*);

	void (* set)(vector*, const int pos, void*);

	void (* pop)(vector*);

	void (* fill)(vector*, void*);

	void (* merge)(vector*, vector*);

	void (* reverse)(vector*);

	iterator* (* iterator)(vector*);
};

vector* create_vector();

vector* create_vector_sized(size_t size, void* val);
vector* create_vector_reserved(size_t size);

vector* create_vector_with(void** values, size_t size);
vector* create_vector_from(int args, ...);

void free_vector(vector* self);
void free_vector_no_values(vector* self);

typedef struct lnode lnode;
struct lnode {
	lnode* prev, * next;
	void* val;
};

typedef struct lvector lvector;

struct lvector {
	lnode* first, * last;
	size_t size;

	void* (* get)( lvector*, const int);

	void* (* get_first)( lvector*);

	void* (* get_last)( lvector*);

	void (* push)(lvector*, void*);
	void (* push_first)(lvector*, void*);

	void (* pop)(lvector*);
	void (* pop_first)(lvector*);
	iterator* (* iterator)(lvector*);
};

void merge_nodes(lvector* list, lnode *firstNode, lnode *nextNode, void* val);
void split_node(lvector* list, lnode *node, void* val1, void *val2);
void extract_node(lvector* list, lnode *node);
lvector* create_lvector();
lvector* create_lvector_with(void** values, size_t size);
lvector* create_lvector_from(int args, ...);

void free_lvector(lvector* self);
void free_lvector_no_values(lvector* self);

vector* cast_ltovector(lvector* vect);
lvector* cast_vtolector(vector* vect);

void arrcopy(void** dest, void** src, size_t size);
void vectcopy(vector* dest, vector* src);

#endif //GW_BASIC_SERVER_VECTOR_H
