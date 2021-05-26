//
// Created by danil on 02.05.2021.
//

#include "json_helper.h"


json_value *get_by_name(const json_value *val, const char *name) {
	if(val){
		size_t length = val->u.object.length;
		for (int i = 0; i < length; ++i)
			if (strcmp(val->u.object.values[i].name, name) == 0) {
				return val->u.object.values[i].value;
			}
	}
	return json_null_new();
}

json_value *create_range(int start_line, int start_pos, int end_line, int end_pos){

	json_value *range = json_object_new(2);

	json_value *start_obj = json_object_new(2);
	json_object_push(start_obj, "line", json_integer_new(start_line));
	json_object_push(start_obj, "character", json_integer_new(start_pos));
	json_object_push(range, "start", start_obj);

	json_value *end_obj = json_object_new(2);
	json_object_push(end_obj, "line", json_integer_new(end_line));
	json_object_push(end_obj, "character", json_integer_new(end_pos));
	json_object_push(range, "end", end_obj);

	return range;
}