cmake_minimum_required(VERSION 3.17)
project(gw_basic_server C)

set(CMAKE_C_STANDARD 11)

add_definitions(-DMY_DEBUG -DHAVE_CONFIG_H)
add_definitions(-DLINUX)
#add_definitions(-DWINDOWS)
add_executable(gw_basic_server server/src/main.c
        server/src/message.c server/src/message.h
        server/src/cstring.c server/src/cstring.h
        server/src/logger.c server/src/logger.h
        server/src/language_server.c server/src/language_server.h
        server/src/json.c server/src/json.h
        server/src/json-builder.c server/src/json-builder.h server/src/types.h server/src/map.c server/src/map.h server/src/vector.c server/src/vector.h server/src/types.c server/src/parser.c server/src/parser.h server/src/json_helper.c server/src/json_helper.h server/src/debug.h server/src/debug.c server/src/diagnostic_messages.c server/src/diagnostic_messages.h
        server/src/config.h server/src/pcre.h server/src/pcre_chartables.c server/src/pcre_internal.h server/src/ucp.h
        server/src/pcre_byte_order.c server/src/pcre_chartables.c server/src/pcre_compile.c server/src/pcre_config.c server/src/pcre_dfa_exec.c server/src/pcre_exec.c server/src/pcre_fullinfo.c server/src/pcre_get.c server/src/pcre_globals.c server/src/pcre_jit_compile.c server/src/pcre_maketables.c server/src/pcre_newline.c server/src/pcre_ord2utf8.c server/src/pcre_refcount.c server/src/pcre_string_utils.c server/src/pcre_study.c server/src/pcre_tables.c server/src/pcre_ucd.c server/src/pcre_valid_utf8.c server/src/pcre_version.c server/src/pcre_xclass.c server/src/tokenizer.c server/src/tokenizer.h)
target_link_libraries(gw_basic_server m)
include_directories(server/src)

#find_library(PCRE_LIBRARY pcre)
#target_link_libraries(gw_basic_server ${PCRE_LIBRARY})

