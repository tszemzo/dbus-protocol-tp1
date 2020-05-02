#include "client.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR 1
#define EXITO 0
#define CHUNK_SIZE 32

bool set_remote_address(struct addrinfo *hints, struct addrinfo **client_info, 
const char *host, const char *service) {

	memset(hints, 0, sizeof(struct addrinfo));
   	hints->ai_family = AF_INET;       // IPv4 (or AF_INET6 for IPv6)
   	hints->ai_socktype = SOCK_STREAM; // TCP  (or SOCK_DGRAM for UDP) 
   	hints->ai_flags = 0;              // None

   	int s;
   	if ((s = getaddrinfo(host, service, hints, client_info)) != 0) { 
      printf("Error in getaddrinfo: %s\n", gai_strerror(s));
      return false;
	}
	printf("%d\n", s); 
	printf("Remote address successfully setted..\n"); 
	return true;
}

bool client_connect(socket_t *s, struct addrinfo *client_info) {
	if (!socket_create(s, client_info)) {
		printf("Error: %s\n", strerror(errno));
      	freeaddrinfo(client_info);
		return false;
	}

	if(!socket_connect(s, client_info)) {
		printf("Error: %s\n", strerror(errno));
		freeaddrinfo(client_info);
		socket_destroy(s);
		return false;
	}

	printf("Socket connected to the remote address..\n");
	return true;
}

bool send_data(socket_t *s, char *data) {
	printf("Data: %s\n", data);
	char buffer[CHUNK_SIZE];
  	memset(buffer, 0, CHUNK_SIZE);
  	bool data_sent;

  	// Aca falta pasar data al buffer
	data_sent = socket_send(s, data, sizeof(buffer));
	printf("Data sent?... %d\n", data_sent);
	if(!data_sent) return false;
	printf("Data correctly sent..\n");
	return true;	
}

void client_destroy(socket_t *s) {
  	socket_shutdown(s);
  	socket_destroy(s);
}

bool client_run(const char *host, const char *service, char *data) {
	struct addrinfo hints;
   	struct addrinfo *client_info;
   	socket_t s;

   	if (!set_remote_address(&hints, &client_info, host, service)) {
   	 	return ERROR;
   	}

	if (!client_connect(&s, client_info)) {
		return ERROR;
	}

	bool sent = send_data(&s, data);
    client_destroy(&s);

    if (!sent) return ERROR;
    return EXITO;
}
