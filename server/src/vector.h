//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_VECTOR_H
#define GW_BASIC_SERVER_VECTOR_H

#include <glob.h>
#include <stdlib.h>
#include <stdarg.h>

#define DEFAULT_VECTOR_SIZE 16
#define VECTOR_EXPANSION_CONSTANT 2

typedef struct vector vector;

struct vector {
	void** items;
	size_t size, _size;

	void* (* get)(const vector*, const int);

	void (* push)(vector*, void*);

	void (*set)(vector *, const int pos, void*);

	void (* pop)(vector*);

	void (*fill)(vector*, void*);
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
	lnode  *first, *last;
	size_t size;

	void* (* get)(const lvector*, const int);

	void* (* get_first)(const lvector*);

	void* (* get_last)(const lvector*);

	void (* push)(lvector*, void*);
	void (* push_first)(lvector*, void*);

	void (*pop)(lvector*);
	void (*pop_first)(lvector*);
};

lvector* create_lvector();
lvector* create_lvector_with(void* *values, size_t size);
lvector* create_lvector_from(int args, ...);

void free_lvector(lvector* self);
void free_lvector_no_values(lvector* self);

void arrcopy(void* *dest, void* *src, size_t size);
void vectcopy(vector *dest, vector *src);

#endif //GW_BASIC_SERVER_VECTOR_H