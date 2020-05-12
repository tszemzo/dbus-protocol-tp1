#include "client.h"
#include "dbus.h"
#include "file_reader.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR 1
#define EXITO 0
#define CHUNK_SIZE 120
#define SERVER_RESPONSE_SIZE 3

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

	return true;
}

bool send_data(socket_t *s, char *data, dbus_t *dbus) {
  	bool data_sent;
  	unsigned char *response;
  	
	response = dbus_encode_line(dbus, data, strlen(data));
	int header_length = dbus_header_length(dbus);
	int body_length = dbus_body_length(dbus);

	data_sent = socket_send(s, response, header_length);
	if(!data_sent) return false;

	data_sent = socket_send(s, &response[header_length], body_length);
	if(!data_sent) return false;

	return true;	
}

void receive_data(socket_t *s, char* buffer, int size) {
   	int received_bytes = 0;
 	memset(buffer, 0, size); 
 	received_bytes = 0;
 	// Leeme size bytes y storeamelos en buffer
 	socket_receive(s, buffer, size, &received_bytes);
}

void client_destroy(socket_t *s) {
  	socket_shutdown(s);
  	socket_destroy(s);
}

void client_run(const char *host, const char *service, char *filename) {
	struct addrinfo hints;
   	struct addrinfo *client_info;
   	socket_t s;
   	dbus_t dbus;
   	file_reader_t file_reader;
   	char *send_buffer = {0};
   	char receive_buffer[SERVER_RESPONSE_SIZE];

   	if (!set_remote_address(&hints, &client_info, host, service)) return;
	if (!client_connect(&s, client_info)) return;
	if (!file_reader_create(&file_reader, filename)) return;
	dbus_create(&dbus);

	while(true) {
		send_buffer = file_reader_get_line(&file_reader);
		if (send_buffer == NULL) return;
		int id = dbus_id(&dbus);
		bool sent = send_data(&s, send_buffer, &dbus);
		if (!sent) { 
            free(send_buffer);
            break;
        }
		receive_data(&s, receive_buffer, SERVER_RESPONSE_SIZE);
		printf("0x%08d: %s\n", id, receive_buffer);
	}
	client_destroy(&s);
	file_reader_destroy(&file_reader);
}
