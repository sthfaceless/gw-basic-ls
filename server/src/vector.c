//
// Created by danil on 27.04.2021.
//

#include "vector.h"

static void shrink_vector(vector* self) {
	if(!self->_size)
		self->_size = 1;
	self->_size *= VECTOR_EXPANSION_CONSTANT;
	self->items = realloc(self->items, self->_size*sizeof(void*));
}

static void* get(const vector* self, const int pos) {
	if (self->items && pos<self->size)
		return *(self->items+pos);
	return NULL;
}

static void* last(const vector* self) {
	return get(self, self->size-1);
}

static void push(vector* self, void* val) {
	if (self->size==self->_size)
		shrink_vector(self);
	*(self->items+self->size) = val;
	self->size++;
}

static void set(vector* self, const int pos, void* val) {
	if (pos<self->size) {
		*(self->items+pos) = val;
	}
}

static void pop(vector* self) {
	if (self->size>0) {
		*(self->items+self->size-1) = NULL;
		self->size--;
	}
}

static void fill(vector* self, void* val) {
	for (void** ptr = self->items; ptr<self->items+self->size; ptr++)
		*ptr = val;
}

vector* merge_vectors(vector* first_vector, vector* second_vector){
	while (first_vector->size+second_vector->size > first_vector->_size)
		shrink_vector(first_vector);
	memcpy(first_vector->items+first_vector->size, second_vector->items, second_vector->size*sizeof (void *));
	first_vector->size += second_vector->size;
	free_vector_no_values(second_vector);
	return first_vector;
}

static void merge(vector* self, vector* oth) {
	merge_vectors(self, oth);
}


static void reverse(vector* self) {
	for (void** l = self->items, ** r = self->items+self->size-1; l<r; l++, r--)
		swap(l, r);
}

static int has_next(iterator* self) {
	vector* vect = (vector*)self->_iterable;
	return self->curr<vect->size;
}

static void* get_next(iterator* self) {
	vector* vect = (vector*)self->_iterable;
	return vect->get(vect, self->curr++);
}

static iterator* get_iterator(vector* self) {
	iterator* it = malloc(sizeof(iterator));
	it->_iterable = self;
	it->curr = 0;
	it->has_next = has_next;
	it->get_next = get_next;
	return it;
}

vector* create_vector_sized(size_t size, void* val) {
	vector* vect = malloc(sizeof(vector));

	vect->items = malloc(size*sizeof(void*));
	vect->_size = size;
	vect->size = size;

	fill(vect, val);

	vect->push = push;
	vect->set = set;
	vect->get = get;
	vect->last = last;
	vect->pop = pop;
	vect->fill = fill;
	vect->merge = merge;
	vect->reverse = reverse;
	vect->iterator = get_iterator;

	return vect;
}

vector* create_vector_with(void** values, size_t size) {
	vector* vect = create_vector_sized(size, NULL);
	arrcopy(vect->items, values, size);
	return vect;
}

vector* create_vector_from(int args, ...) {
	va_list list;
	va_start(list, args);

	void** values = malloc(args*sizeof(void*));
	for (int i = 0; i<args; ++i) {
		*(values+i) = va_arg(list, void*);
	}

	va_end(list);
	return create_vector_with(values, args);
}

vector* create_vector_reserved(size_t size) {
	vector* vect = create_vector_sized(size, NULL);
	vect->size = 0;
	return vect;
}

vector* create_vector() {
	return create_vector_reserved(DEFAULT_VECTOR_SIZE);
}

void free_vector(vector* self) {
	int cnter = 0;
	for (void** ptr = self->items; ptr<self->items+self->size; ++ptr) {
		if (*ptr)
			free(*ptr), cnter++;
	}
	free(self->items);
	free(self);
}

void free_vector_no_values(vector* self) {
	free(self->items);
	free(self);
}

void* get_first(lvector* self) {
	return self->first ? self->first->val : NULL;
}

void* get_last(lvector* self) {
	return self->last ? self->last->val : NULL;
}

void* getl(lvector* self, const int pos) {
	lnode* curr = get_first(self);
	int i = 0;
	while (curr && i!=pos) {
		curr = curr->next;
		i++;
	}
	return curr->val;
}

static lnode* create_lnode(void* val) {
	lnode* node = malloc(sizeof(lnode));
	node->val = val;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

void pushl(lvector* self, void* val) {

	lnode* node = create_lnode(val);

	node->prev = self->last;
	if (self->last)
		self->last->next = node;
	self->last = node;

	if (!self->first)
		self->first = node;

	self->size++;
}

void push_first(lvector* self, void* val) {

	lnode* node = create_lnode(val);

	node->next = self->first;
	if (self->first)
		self->first->prev = node;
	self->first = node;

	if (!self->last)
		self->last = node;

	self->size++;
}

void popl(lvector* self) {
	if (self->last) {
		lnode *curr = self->last, *prev = curr->prev;
		if (prev) {
			self->last = prev;
			prev->next = NULL;
		}
		else {
			self->first = self->last = NULL;
		}
		self->size--;
	}
}

void pop_first(lvector* self) {
	if (self->first) {
		lnode *curr = self->first, *next = self->first->next;
		if (next) {
			self->first = next;
			next->prev = NULL;
		}
		else {
			self->first = self->last = NULL;
		}
		self->size--;
	}
}

static int has_lnext(iterator* self) {
	lnode* node = (lnode*)self->_iterable;
	return node!=NULL;
}

static void* get_lnext(iterator* self) {
	if (self->_iterable) {
		lnode* node = (lnode*)self->_iterable;
		self->_iterable = node->next;
		self->curr++;
		return node->val;
	}
	return NULL;
}

static iterator* get_literator(lvector* self) {
	iterator* it = malloc(sizeof(iterator));
	it->_iterable = self->first;
	it->curr = 0;
	it->has_next = has_lnext;
	it->get_next = get_lnext;
	return it;
}

void reverse_merge_nodes(lvector *list, lnode *firstNode, lnode *secondNode, void* val){
	lnode *prev = firstNode->prev;

	if(firstNode == list->first)
		list->first = secondNode;

	secondNode->val = val;
	secondNode->prev = prev;
	if(prev)
		prev->next = secondNode;

	list->size--;
	free(firstNode);
}

void merge_nodes(lvector *list, lnode *firstNode, lnode *secondNode, void* val){
	lnode *next = secondNode->next;

	if(secondNode == list->last)
		list->last = firstNode;

	firstNode->val = val;
	firstNode->next = next;
	if(next)
		next->prev = firstNode;

	list->size--;
	free(secondNode);
}

void split_node(lvector* list, lnode *node, void* val1, void *val2){

	lnode* node2 = create_lnode(val2);
	node2->next = node->next;
	node2->prev = node;
	if(list->last == node)
		list->last = node2;

	node->val = val1;
	node->next = node2;

	list->size++;
}

void extract_node(lvector* list, lnode *node){

	lnode *prev = node->prev, *next = node->next;

	if(list->first == node)
		list->first = next;
	if(list->last == node)
		list->last = prev;

	if(next) next->prev = prev;
	if(prev) prev->next = next;

	free(node);
	list->size--;
}

lvector* create_lvector() {
	lvector* vect = malloc(sizeof(lvector));

	vect->size = 0;
	vect->first = vect->last = NULL;

	vect->get = getl;
	vect->get_first = get_first;
	vect->get_last = get_last;
	vect->push = pushl;
	vect->push_first = push_first;
	vect->pop = popl;
	vect->pop_first = pop_first;
	vect->iterator = get_literator;

	return vect;
}

lvector* create_lvector_with(void** values, size_t size) {
	lvector* vect = create_lvector();
	for (void** ptr = values; ptr<values+size; ptr++) {
		vect->push(vect, *ptr);
	}
	return vect;
}
lvector* create_lvector_from(int args, ...) {
	va_list list;
	va_start(list, args);

	void** values = malloc(args*sizeof(void*));
	for (int i = 0; i<args; ++i) {
		*(values+i) = va_arg(list, void*);
	}

	va_end(list);
	return create_lvector_with(values, args);
}

void free_lvector(lvector* self) {
	while (self->last) {
		free(self->last->val);
		popl(self);
	}
	free(self);
}

void free_lvector_no_values(lvector* self) {
	while (self->first) {
		popl(self);
	}
	free(self);
}

vector* cast_ltovector(lvector* vect) {
	vector* result = create_vector();
	iterator* it = vect->iterator(vect);
	while (it->has_next(it))
		result->push(result, it->get_next(it));
	free_lvector_no_values(vect);
	return result;
}
lvector* cast_vtolector(vector* vect) {
	lvector* result = create_lvector();
	iterator* it = vect->iterator(vect);
	while (it->has_next(it))
		result->push(result, it->get_next(it));
	free_vector_no_values(vect);
	return result;
}

void arrcopy(void** dest, void** src, size_t size) {
	void** end = src+size;
	for (void** dest_ptr = dest, ** src_ptr = src; src_ptr<end; dest_ptr++, src_ptr++)
		*dest_ptr = *src_ptr;
}

void vectcopy(vector* dest, vector* src) {
	arrcopy(dest->items, src->items, src->size);
}