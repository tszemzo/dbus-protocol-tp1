#include "client.h"
#include "dbus.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR 1
#define EXITO 0
#define CHUNK_SIZE 120

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

bool send_data(socket_t *s, FILE *data) {
	char line[CHUNK_SIZE];
  	bool data_sent;
  	unsigned char *response;

  	dbus_t dbus;
  	dbus_create(&dbus);

  	// By the moment lets suppose that theres a unique line
  	while (!feof(data)) {
		memset(line, 0, CHUNK_SIZE);
		int read = fread(line, 1, CHUNK_SIZE - 1, data);
		if (read != CHUNK_SIZE-1 && !feof(data)) {
			printf("Error reading the file.\n");
			return false;
		}		
	}
	printf("Line... %s\n", line);
	response = parse_line(&dbus, line, strlen(line));
	int header_length = dbus_header_length(&dbus);
	int body_length = dbus_body_length(&dbus);

	data_sent = socket_send(s, response, header_length);
	if(!data_sent) return false;
	printf("Data sent [Should be 1]?... %d\n", data_sent);

	data_sent = socket_send(s, &response[header_length], body_length);
	if(!data_sent) return false;
	printf("Data sent 2 [Should be 1]?... %d\n", data_sent);

	return true;	
}

void client_destroy(socket_t *s) {
  	socket_shutdown(s);
  	socket_destroy(s);
}

bool client_run(const char *host, const char *service, char *filename) {
	struct addrinfo hints;
   	struct addrinfo *client_info;
   	socket_t s;

   	if (!set_remote_address(&hints, &client_info, host, service)) return ERROR;
	if (!client_connect(&s, client_info)) return ERROR;

	FILE *data = fopen(filename, "r");
	if (!data) {
		printf("Error: could not open the file");
		return ERROR;
	}

	bool sent = send_data(&s, data);
    client_destroy(&s);
    fclose(data);

    if (!sent) return ERROR;
    return EXITO;
}
