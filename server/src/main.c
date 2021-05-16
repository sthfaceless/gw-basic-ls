
#include <stdio.h>
#include <stdlib.h>

#include "json-builder.h"
#include "json.h"
#include "logger.h"
#include "message.h"
#include "language_server.h"
#include "cstring.h"
#include "vector.h"
#include "parser.h"

#define LOG_FILE_PATH "/home/danil/CLionProjects/gw_basic_server/sandbox/log.txt"
#define CONFIG_FILE_PATH "data/config.json"

extern logger* Logger;

void debugi(int val){
	printf("%d\n", val);
}

void debug(void* str){
	if(str)
		printf("%s\n", (char *) str);
}

int main(int argc, char * argv[]) {

	initialize_logger(LOG_FILE_PATH);

    message * msg = create_message();
    language_server *ls = create_language_server();

    FILE *fptr = fopen("/home/danil/CLionProjects/gw_basic_server/sandbox/log2.txt", "a");

    while (1){

        int c = getchar();

        if(c >= 0){

            fprintf(fptr, "%c", c);
            fflush(fptr);

            msg->add_char(msg, c);

            if(msg->is_finished(msg)){
                if(!ls->process(ls, msg->get_message(msg), msg->size))
                    break;
                msg->clear_message(msg);
            }
        }

    }

    fclose(fptr);

    free_language_server(ls);
    free_message(msg);
    free_logger();

    return 0;
}
