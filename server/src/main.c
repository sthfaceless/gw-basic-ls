
#include <stdio.h>
#include <stdlib.h>

#include "json-builder.h"
#include "json.h"
#include "logger.h"
#include "message.h"
#include "language_server.h"

#define LOG_FILE_PATH "/home/danil/tmp/log.txt"

extern logger* Logger;

int main(int argc, char * argv[]) {

    initialize_logger(LOG_FILE_PATH);
    char * c_buff = malloc(sizeof(char)*2);

    message * msg = create_message();

    language_server *ls = create_language_server();

    while (1){

        char c = getchar();
        if(c >= 0){
            msg->add_char(msg, c);

            if(msg->is_finished(msg)){
                Logger->log(log_info, msg->get_message(msg));
                if(!ls->process(ls, msg->get_message(msg), msg->size))
                    break;
                msg->clear_message(msg);
            }
        }

    }

    free_language_server(ls);
    free_message(msg);
    free_logger();
    free(c_buff);

    return 0;
}
