//
// Created by danil on 25.05.2021.
//

#include "debug.h"

int connect_d = -1;

void debugi(int val) {
	printf("%d\n", val);
}

void debug(void* str) {
	if (str)
		printf("%s\n", (char*)str);
}

void init_debug() {

	int listener_d  = socket(PF_INET, SOCK_STREAM, 0);
	if(listener_d == -1)
		exit(1);

	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t) htons(6009);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listener_d, (struct sockaddr *) &name, sizeof (name)) == -1)
		exit(1);
	listen(listener_d, 5);

	while (connect_d == -1){
		struct sockaddr_storage cliend_addr;
		unsigned int address_size = sizeof (cliend_addr);
		connect_d = accept(listener_d, (struct sockaddr *) & cliend_addr, &address_size);
	}
}

void fprintfsockf(FILE *restrict ptr, const char*restrict f, ... ){

	va_list a;
	va_start( a, f );
	int l = vsnprintf( 0, 0, f, a );
	char* buf = (char*) malloc( l + 1 );
	va_start( a, f );
	vsnprintf( buf, l, f, a );

	send( connect_d, buf, l, 0 );

	free( buf );
}

char read_in(){
	char *buf = malloc(sizeof (char) * 2);
	int slen = 1;
	int c = recv(connect_d, buf, slen, 0);
	while(c != 1){
		c = recv(connect_d, buf, slen, 0);
	}
	printf("%c", *buf);
	return *buf;
}

