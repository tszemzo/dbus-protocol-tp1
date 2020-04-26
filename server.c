#include "server.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define SUCCESS 0
#define ERROR 1
#define PORT "8080"
#define BACKLOG 10

bool set_local_address(struct addrinfo *hints, struct addrinfo **server_info, const char *service) {

	memset(hints, 0, sizeof(struct addrinfo));
   	hints->ai_family = AF_INET;       // IPv4     
   	hints->ai_socktype = SOCK_STREAM; // TCP 
   	hints->ai_flags = AI_PASSIVE;     // AI_PASSIVE for server 

   	int s;

	if ((s = getaddrinfo(NULL, service, hints, server_info)) != 0) {
		printf("Error in getaddrinfo: %s\n", gai_strerror(s));
		return false;
	}
	printf("%d\n", s); 
	printf("Local address successfully setted..\n"); 
	return true;
}

bool bind_and_listen(socket_t *s, struct addrinfo *server_info, socket_t *new_s) {
	if (!socket_create(s, server_info)) {
		printf("Error: %s\n", strerror(errno));
      	freeaddrinfo(server_info);
		return false;
	}
	printf("Socket ready for binding..\n");
	return true;
}


bool create_server(const char *service) {
	struct addrinfo hints;
   	struct addrinfo *server_info;

   	if (!set_local_address(&hints, &server_info, service)) return ERROR;

   	socket_t s;
	socket_t new_s;

	if (!bind_and_listen(&s, server_info, &new_s)) return ERROR;

	return SUCCESS;
}