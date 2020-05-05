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
#define METADATA_SIZE 16
#define HEADER_SIZE_POSITION 12
#define BODY_SIZE_POSITION 4
#define SERVER_RESPONSE_SIZE 3

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

bool send_response(socket_t *client_s) {
  	bool data_sent;
  	unsigned char response[SERVER_RESPONSE_SIZE];
  	response[0] = 'O';
  	response[1] = 'K';
  	response[2] = '\n';

	data_sent = socket_send(client_s, response, SERVER_RESPONSE_SIZE);
	if(!data_sent) return false;
	return true;
}

void receive_message(socket_t *s, char* buffer, int size) {
   	int received_bytes = 0;
 	memset(buffer, 0, size); 
 	received_bytes = 0;
 	// Leeme size bytes y storeamelos en buffer
 	socket_receive(s, buffer, size, &received_bytes);
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

	// This is going to be encapsulated
	char message_buffer[METADATA_SIZE];
	receive_message(&client_s, message_buffer, METADATA_SIZE);
	int content_size = message_buffer[BODY_SIZE_POSITION] + message_buffer[HEADER_SIZE_POSITION];
	char content_buffer[content_size];
	receive_message(&client_s, content_buffer, content_size);

	if (!send_response(&client_s)) return ERROR;

	server_destroy(&s, &client_s);
	return SUCCESS;
}