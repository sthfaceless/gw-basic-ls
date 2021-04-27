//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_VECTOR_H
#define GW_BASIC_SERVER_VECTOR_H

#include <glob.h>

typedef struct vector vector;
struct vector{
    void* *items;
    size_t size, _size;
    void* (*get)(const vector *, const int);
    void (*add)(vector *,const void*);
};

vector * create_vector();
vector * create_vector_sized(size_t size);
vector * create_vector_from(const void* *values, size_t size);
vector * free_vector(vector * self);


typedef struct lnode lnode;
struct lnode{
    lnode *prev, *next;
    void* val;
};

typedef struct lvector lvector;
struct lvector{
    lnode *first, *last;
    size_t size;
    void* (*get)(const lvector*, const int);
    void* (*get_first)(const lvector*);
    void* (*get_last)(const lvector*);
    void (*add)(lvector*, void*);
};

lvector * create_lvector();
lvector * free_lvector(lvector * self);

#endif //GW_BASIC_SERVER_VECTOR_H
