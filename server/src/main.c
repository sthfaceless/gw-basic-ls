
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

#include "json-builder.h"
#include "json.h"
#include "logger.h"
#include "message.h"
#include "language_server.h"
#include "cstring.h"
#include "vector.h"
#include "parser.h"
#include "cstring.h"

char* LOG_FILE = "";
char* CONFIG_FILE = "";

extern logger* Logger;

void parse_args(int argc, char* argv[]) {

	for (int i = 1; i<argc; i++) {
		vector* values = strsplit(argv[i], '=');
		if (values->size==2)
			if (!strcmp((char*)values->get(values, 0), "config_file"))
				CONFIG_FILE = (char*)values->get(values, 1);
			else if (!strcmp((char*)values->get(values, 0), "log_file"))
				LOG_FILE = (char*)values->get(values, 1);
	}

}

int main(int argc, char* argv[]) {

	parse_args(argc, argv);

	initialize_logger(LOG_FILE);

	message* msg = create_message();
	language_server* ls = create_language_server(CONFIG_FILE);

	FILE* fptr = fopen("/home/danil/CLionProjects/gw_basic_server/log2.txt", "a");

	while (1) {

		int c = getchar();

		if (c>=0) {

			fprintf(fptr, "%c", c);
			fflush(fptr);

			msg->add_char(msg, c);

			if (msg->is_finished(msg)) {
				if (!ls->process(ls, msg->get_message(msg), msg->size))
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
