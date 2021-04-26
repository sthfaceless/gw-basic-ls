cmake_minimum_required(VERSION 3.17)
project(gw_basic_server C)

set(CMAKE_C_STANDARD 11)


add_executable(gw_basic_server server/src/main.c
        server/src/message.c server/src/message.h
        server/src/cstring.c server/src/cstring.h
        server/src/logger.c server/src/logger.h
        server/src/language_server.c server/src/language_server.h
        server/src/json.c server/src/json.h
        server/src/json-builder.c server/src/json-builder.h)
target_link_libraries(gw_basic_server m)
include_directories(server/src)
