#include <stdio.h>
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

bool set_local_address(struct addrinfo *hints, struct addrinfo **ptr, const char *service) {

	memset(hints, 0, sizeof(struct addrinfo));
   	hints->ai_family = AF_INET;       // IPv4     
   	hints->ai_socktype = SOCK_STREAM; // TCP 
   	hints->ai_flags = AI_PASSIVE;     // AI_PASSIVE for server 

   	int s;

	if ((s = getaddrinfo(NULL, service, hints, ptr)) != 0) {
		printf("Error in getaddrinfo: %s\n", gai_strerror(s));
		return false;
	}
	printf("%d\n", s); 
	printf("Local address successfully setted..\n"); 
	return true;
}


bool create_server(const char *service) {
	struct addrinfo hints;
   	struct addrinfo *ptr;

   	if (!set_local_address(&hints, &ptr, service)) return ERROR;

	return SUCCESS;
}