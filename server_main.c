#include "socket.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR 1

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: ./server <puerto>\n");
    	return ERROR;
    }

    char* service = argv[1];
    server_run(service);
	return SUCCESS;
}