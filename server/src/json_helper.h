//
// Created by danil on 02.05.2021.
//

#ifndef GW_BASIC_SERVER_JSON_HELPER_H
#define GW_BASIC_SERVER_JSON_HELPER_H

#include "stdlib.h"
#include "string.h"
#include "json.h"
#include "json-builder.h"

json_value *get_by_name(const json_value* val, const char* name);
json_value *create_range(int start_line, int start_pos, int end_line, int end_pos);

#endif //GW_BASIC_SERVER_JSON_HELPER_H
