cmake_minimum_required(VERSION 3.17)
project(gw_basic_server C)

set(CMAKE_C_STANDARD 11)


add_definitions(-DMY_DEBUG)
add_executable(gw_basic_server server/src/main.c
        server/src/message.c server/src/message.h
        server/src/cstring.c server/src/cstring.h
        server/src/logger.c server/src/logger.h
        server/src/language_server.c server/src/language_server.h
        server/src/json.c server/src/json.h
        server/src/json-builder.c server/src/json-builder.h server/src/types.h server/src/map.c server/src/map.h server/src/vector.c server/src/vector.h server/src/types.c server/src/parser.c server/src/parser.h server/src/json_helper.c server/src/json_helper.h server/src/debug.h server/src/debug.c server/src/diagnostic_messages.c server/src/diagnostic_messages.h)
target_link_libraries(gw_basic_server m)
include_directories(server/src)

find_library(PCRE_LIBRARY pcre)
target_link_libraries(gw_basic_server ${PCRE_LIBRARY})

