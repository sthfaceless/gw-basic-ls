//
// Created by danil on 27.04.2021.
//

#ifndef GW_BASIC_SERVER_MAP_H
#define GW_BASIC_SERVER_MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "math.h"
#include "types.h"
#include "vector.h"

#define INIT_HASH_VALUE 5009
#define DEFAULT_MAP_SIZE 32
#define MAP_EXPANSION_CONSTANT 2
#define MAP_SHRINK_CONSTANT 1.5
#define MAP_VECTOR_STEP 7

typedef ull (*hasher)(void*);
hasher get_hasher(type_t key_t);

typedef struct map_entry map_entry;
struct map_entry{
	void *key, *val;
	int cnt;
};

typedef struct map map;
struct map{
	vector *entries;
	size_t _size, size;
	hasher _hasher;
	comp _comp;
	void (*put)(map*, void*, void*);
	void* (*get)(map*, void*);
	void (*erase)(map*, void*);
	lvector* (*entry_list)(map *);
	int (*count)(map*, void*);
};

map *create_map(type_t key_t);
map *create_custom_map(hasher _hasher, comp _comp);
map* create_custom_map_sized(hasher _hasher, comp _comp, size_t size);
void free_map(map* self);
void free_map_no_values(map* self);

typedef struct tnode tnode;
struct tnode{
	void *key, *val;
	int priority, cnt;
	tnode *l, *r;
};

typedef struct tmap tmap;
struct tmap{
	tnode *root;
	size_t size;
	comp _comp;
	void (*put)(tmap*, void*, void*);
	void* (*get)(tmap*, void*);
	void (*erase)(tmap*, void*);
	lvector* (*entry_list)(tmap *);
	int (*count)(tmap*, void*);
};

tmap* create_tmap(type_t key_t);
tmap* create_custom_tmap(comp _comp);
void free_tmap(tmap *self);
void free_tmap_no_values(tmap *self);

#endif //GW_BASIC_SERVER_MAP_H
