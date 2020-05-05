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
#define ACCEPT_QUEUE_LENGTH 10
#define CHUNK_SIZE 32

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

bool bind_and_listen(socket_t *s, struct addrinfo *server_info) {
	if (!socket_create(s, server_info)) {
		printf("Error: %s\n", strerror(errno));
      	freeaddrinfo(server_info);
		return false;
	}
	printf("Socket ready for binding..\n");

	if (!socket_bind_and_listen(s, server_info, ACCEPT_QUEUE_LENGTH)) {
		printf("Error: %s\n", strerror(errno));
		return false;
	}
	printf("Socket binded and ready for accept..\n");

	return true;
}

bool server_accept(socket_t *s, socket_t *client_s) {
	if (!socket_accept(s, client_s)) {
		printf("Error: %s\n", strerror(errno));
		return false;
	}
	return true;
}

bool receive_message(socket_t *s) {
	char buffer[CHUNK_SIZE];
   	int received_bytes = 0;

   	// Mientras haya conexion
  	while (received_bytes >= 0) {
  	 	memset(buffer, 0, CHUNK_SIZE); 
  	 	received_bytes = 0;
  	 	// Leeme CHUNK_SIZE bytes y storeamelos en buffer
  	 	if(!socket_receive(s, buffer, CHUNK_SIZE, &received_bytes)) return false;
  	}
  	return true;
}

void server_destroy(socket_t *s, socket_t *client_s) {
	socket_shutdown(client_s);
  	socket_destroy(client_s);
  	socket_shutdown(s);
  	socket_destroy(s);
}

bool server_run(const char *service) {
	struct addrinfo hints;
   	struct addrinfo *server_info;
   	socket_t s;
	socket_t client_s;

   	if (!set_local_address(&hints, &server_info, service)) return ERROR;
	if (!bind_and_listen(&s, server_info)) return ERROR;
	if (!server_accept(&s, &client_s)) return ERROR;
	if (!receive_message(&client_s)) return ERROR;
	// if (!send_response(&client_s)) return ERROR;

	server_destroy(&s, &client_s);
	return SUCCESS;
}