#include "socket.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define SUCCESS 0
#define ERROR 1
#define MIN_PARAMS 3
#define HOST 1
#define SERVICE 2
#define FILE 3

int main(int argc, char** argv) {
	if (argc < MIN_PARAMS) {
        printf("Uso: ./client <host> <puerto> [<archivo de entrada>]\n");
    	return ERROR;
    }
    char* host = argv[HOST];
    char* service = argv[SERVICE];
    char* filename = argv[FILE];

    printf("File to read: %s\n", filename);
    client_run(host, service, filename);
	return SUCCESS;
}