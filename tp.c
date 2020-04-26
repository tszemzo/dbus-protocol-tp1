#include "socket.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR 1
#define PORT "8080"

int main(int argc, char *argv[]) {

	create_server(PORT);
	return SUCCESS;
}
