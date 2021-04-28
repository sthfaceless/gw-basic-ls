//
// Created by danil on 25.04.2021.
//

#include "cstring.h"

string * get_string(){
    string *str = malloc(sizeof (string));

    str->buff_sz = DEFAULT_BUFF_SIZE;
    str->str = malloc(sizeof (char) * (DEFAULT_BUFF_SIZE + 1));
    str->str[0] = '\0';

    str->str_sz = 0;
    return str;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

vector* match(const char *str, char *pattern){

	static map *pattern_cache = NULL;
	if(pattern_cache == NULL)
		pattern_cache = create_map(STRING);

	void * req_ptr = pattern_cache->get(pattern_cache, pattern);
	if(req_ptr == NULL){
		const char* error;
		int error_offset;
		req_ptr = pcre_compile(pattern, 0, &error, &error_offset, NULL);
		pattern_cache->put(pattern_cache, pattern, req_ptr);
	}
	pcre* req_re = (pcre*) req_ptr;

	vector* vect = create_vector_reserved(2);
	if(req_re != NULL){
		int match_vector[1024];
		int rc = pcre_exec(req_re, NULL, str, strlen(str), 0, 0,
				match_vector, sizeof(match_vector)/sizeof(match_vector[0]));
		if(rc >= 0){
			for (int i = 0; i < rc; ++i) {
				int * pair = malloc(2 * sizeof (int));
				pair[0] = match_vector[2*rc];
				pair[1] = match_vector[2*rc+1];
				vect->push(vect, &pair);
			}
		}
	}

//	pcre_free(req_re);

	return vect;
}
