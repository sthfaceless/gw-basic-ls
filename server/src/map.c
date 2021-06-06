//
// Created by danil on 27.04.2021.
//

#include "map.h"

static const ull MASK4 = (1 << 4)-1;
static const ull TEN_INF = 1000*1000*1000;

static ull _hash(const ull prev, const ull val) {
	return ((prev << 5)+prev)+val;
}

static ull ll_hasher(void* _val) {
	ull val = *((ull*)(_val));
	ull res = INIT_HASH_VALUE;
	while (val > 0) {
		res = _hash(res, val & MASK4);
		val >>= 4;
	}
	return res;
}

static ull ld_hasher(void* _val) {

	ld _mant;
	ll val = (ll)modfl(*(ld*)_val, &_mant);
	ll mant = (ll)(_mant*TEN_INF);

	return _hash(ll_hasher(&val), ll_hasher(&mant));
}

static ull ch_hasher(void* _val) {
	char ch = *((char*)(_val));
	ull res = INIT_HASH_VALUE;
	while (ch > 0) {
		res = _hash(res, ch & MASK4);
		ch >>= 4;
	}
	return res;
}

static ull str_hasher(void* _val) {
	char* str = (char*)_val;
	ull res = INIT_HASH_VALUE;
	while (*str!=0) {
		res = _hash(res, ch_hasher(str));
		str++;
	}
	return res;
}

static ull ptr_hasher(void* _val) {
	return ll_hasher(&_val);
}

hasher get_hasher(type_t key_t) {
	switch (key_t) {
	case INT_KEY:
		return ll_hasher;
	case DOUBLE_KEY:
		return ld_hasher;
	case CHAR_KEY:
		return ch_hasher;
	case STRING_KEY:
		return str_hasher;
	case STRUCT_KEY:
		return ptr_hasher;
	default:
		return ptr_hasher;
	}
}

map_entry* get_entry(void* key, void* val) {
	map_entry* entry = malloc(sizeof(map_entry));
	entry->key = key;
	entry->val = val;
	entry->cnt = 1;
	return entry;
}

static void __put(map* self, map_entry* entry) {
	ull hash = self->_hasher(entry->key)%self->_size;

	map_entry* curr = self->entries->get(self->entries, hash);
	while (curr && self->_comp(curr->key, entry->key))
		hash = (hash+MAP_VECTOR_STEP)%self->_size,
				curr = self->entries->get(self->entries, hash);

	if (curr) {
		free(curr->val);
		curr->val = entry->val;
		curr->cnt++;
		free(entry);
	}else {
		self->entries->set(self->entries, hash, entry);
		self->size++;
	}
}

static void _put(map* self, void* key, void* val) {
	__put(self, get_entry(key, val));
}

lvector* entry_list(map* self) {
	lvector* vect = create_lvector();
	for (void** ptr = self->entries->items; ptr<self->entries->items+self->_size; ptr++)
		if (*ptr!=NULL)
			vect->push(vect, *ptr);
	return vect;
}

static int find(map* self, void* key) {
	int count = 0;
	ull hash = self->_hasher(key)%self->_size;

	void* result = self->entries->get(self->entries, hash);
	while (result!=NULL && self->_comp(((map_entry*)result)->key, key) && count<self->_size)
		hash = (hash+MAP_VECTOR_STEP)%self->_size,
				result = self->entries->get(self->entries, hash),
				count++;

	if (count==self->_size || result==NULL)
		return -1;

	else return hash;
}

static void shrink_map(map* self) {

	size_t nsize = (self)->_size * MAP_EXPANSION_CONSTANT;

	lvector* values = entry_list(self);
	free_vector_no_values(self->entries);
	self->size = 0;
	self->entries = create_vector_sized(nsize, NULL);

	while (values->size) {
		__put(self, (map_entry*)values->get_last(values));
		values->pop(values);
	}
	free_lvector_no_values(values);

}

static void put(map* self, void* key, void* val) {

	if (self->size*MAP_SHRINK_CONSTANT>self->_size)
		shrink_map(self);

	_put(self, key, val);
}

static void* get(map* self, void* key) {

	int pos = find(self, key);
	if (pos!=-1)
		return ((map_entry*) self->entries->get(self->entries, pos))->val;

	return NULL;
}
static void erase(map* self, void* key) {
	int pos = find(self, key);
	if (pos!=-1) {
		free(self->entries->get(self->entries, pos));
		self->entries->set(self->entries, pos, NULL);
	}
}

static int count(map* self, void* key) {
	int pos = find(self, key);
	if (pos == -1)
		return 0;
	void* entry = self->entries->get(self->entries, pos);
	return ((map_entry*)entry)->cnt;
}

map* create_custom_map_sized(hasher _hasher, comp _comp, size_t size) {
	map* self = malloc(sizeof(map));

	self->_size = size;
	self->size = 0;
	self->entries = create_vector_sized(self->_size, NULL);

	self->_hasher = _hasher;
	self->_comp = _comp;

	self->get = get;
	self->put = put;
	self->erase = erase;
	self->entry_list = entry_list;
	self->count = count;
}

map* create_custom_map(hasher _hasher, comp _comp) {
	return create_custom_map_sized(_hasher, _comp, DEFAULT_MAP_SIZE);
}
map* create_map(type_t key_t) {
	return create_custom_map(get_hasher(key_t), get_comparator(key_t));
}

void free_map(map* self) {
	free_vector(self->entries);
	free(self);
}
void free_map_no_values(map* self) {
	free_vector_no_values(self->entries);
	free(self);
}

static tnode* create_node(void* key, void* val) {
	tnode* node = malloc(sizeof(tnode));
	node->key = key;
	node->val = val;
	node->cnt = 1;
	node->priority = rand();
	node->l = node->r = NULL;
	return node;
}

static tnode* tfind(tmap* self, void* key) {
	tnode* node = self->root;
	while (node && self->_comp(node->key, key))
		if (self->_comp(key, node->key) < 0)
			node = node->l;
		else
			node = node->r;
	return node;
}

static void split(tmap* self, tnode* t, void* key, tnode** l, tnode** r) {
	if (!t)
		l = r = NULL;
	else if (self->_comp(key, t->key)<0)
		split(self, t->l, key, l, &t->l), *r = t;
	else
		split(self, t->r, key, &t->r, r), *l = t;
}

static void insert(tmap* self, tnode** t, tnode* it) {
	if (!(*t))
		*t = it;
	else if(!self->_comp((*t)->key, it->key)){
		(*t)->cnt++;
		free(it);
		return;
	}
	else if (it->priority>(*t)->priority)
		split(self, *t, it->key, &it->l, &it->r), *t = it;
	else
		insert(self, self->_comp(it->key, (*t)->key)<0 ? &(*t)->l : &(*t)->r, it);
}

static void merge(tmap* self, tnode** t, tnode* l, tnode* r) {
	if (!l || !r)
		*t = l ? l : r;
	else if (l->priority>r->priority)
		merge(self, &l->r, l->r, r), *t = l;
	else
		merge(self, &r->l, l, r->l), *t = r;
}

static void _terase(tmap* self, tnode** t, void* key) {
	if(*t) {
		if (!self->_comp((*t)->key, key)) {
			free(*t);
			self->size--;
			merge(self, t, (*t)->l, (*t)->r);
		}
		else
			_terase(self, self->_comp(key, (*t)->key)<0 ? &(*t)->l : &(*t)->r, key);
	}
}

static tnode* unite(tmap* self, tnode* l, tnode* r) {
	if (!l || !r)
		return l ? l : r;
	if (l->priority<r->priority)
		swap((void**)&l, (void**)&r);
	tnode* lt, * rt;
	split(self, r, l->key, &lt, &rt);
	l->l = unite(self, l->l, lt);
	l->r = unite(self, l->r, rt);
	return l;
}

static void tput(tmap* self, void* key, void* val) {
	insert(self, &self->root, create_node(key, val));
	self->size++;
}
static void* tget(tmap* self, void* key) {
	tnode* node = tfind(self, key);
	if (node)
		return node->val;
	return node;
}
static void terase(tmap* self, void* key) {
	if (self->root)
		_terase(self, &self->root, key);
}
static lvector* tentry_list(tmap* self) {
	lvector* vect = create_lvector(),
			* queue = create_lvector();

	if (self->root)
		queue->push(queue, self->root);

	while (queue->size) {
		tnode* node = (tnode*)queue->get_last(queue);
		queue->pop(queue);
		if (node->l) queue->push(queue, node->l);
		if (node->r) queue->push(queue, node->r);
		vect->push(vect, node);
	}
	free_lvector_no_values(queue);

	return vect;
}
static int tcount(tmap* self, void* key) {
	tnode* node = tfind(self, key);
	if (node)
		return node->cnt;
	return 0;
}

tmap* create_custom_tmap(comp _comp) {
	tmap* self = malloc(sizeof(tmap));

	self->size = 0;
	self->_comp = _comp;
	self->root = NULL;

	self->put = tput;
	self->get = tget;
	self->erase = terase;
	self->entry_list = tentry_list;
	self->count = tcount;

	return self;
}

tmap* create_tmap(type_t key_t) {
	return create_custom_tmap(get_comparator(key_t));
}

void free_tmap(tmap* self) {
	lvector* queue = create_lvector();

	if (self->root)
		queue->push(queue, self->root);

	while (queue->size) {
		tnode* node = (tnode*)queue->get_last(queue);
		queue->pop(queue);
		if (node->l) queue->push(queue, node->l);
		if (node->r) queue->push(queue, node->r);
		free(node->val);
		free(node);
	}
	free_lvector(queue);
	free(self);
}

void free_tmap_no_values(tmap* self) {
	lvector* queue = create_lvector();

	if (self->root)
		queue->push(queue, self->root);

	while (queue->size) {
		tnode* node = (tnode*)queue->get_last(queue);
		queue->pop(queue);
		if (node->l) queue->push(queue, node->l);
		if (node->r) queue->push(queue, node->r);
		free(node);
	}

	free_lvector(queue);
	free(self);
}