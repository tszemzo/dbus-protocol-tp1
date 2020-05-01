#include "socket.h"
#include "client.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR 1

int main(int argc, char** argv) {
    char* host = argv[1];
    char* service = argv[2];
    char data[5] = "Ailin";
    client_run(host, service, data);
	return SUCCESS;
}