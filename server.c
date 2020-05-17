#include "server.h"
#include "dbus.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define SUCCESS 0
#define ERROR 1
#define METADATA_SIZE 16
#define ACCEPT_QUEUE_LENGTH 10
#define HEADER_SIZE_POSITION 12
#define ID_POSITION 8
#define BODY_SIZE_POSITION 4
#define SERVER_RESPONSE_SIZE 3

bool set_local_address(struct addrinfo *hints, struct addrinfo **server_info,
	const char *service) {
	memset(hints, 0, sizeof(struct addrinfo));
   	hints->ai_family = AF_INET;       // IPv4     
   	hints->ai_socktype = SOCK_STREAM; // TCP 
   	hints->ai_flags = AI_PASSIVE;     // AI_PASSIVE for server 

   	int s;
	if ((s = getaddrinfo(NULL, service, hints, server_info)) != 0) {
		printf("Error in getaddrinfo: %s\n", gai_strerror(s));
		return false;
	}
	return true;
}

bool bind_and_listen(socket_t *s, struct addrinfo *server_info) {
	if (!socket_create(s, server_info)) {
		printf("Error: %s\n", strerror(errno));
      	freeaddrinfo(server_info);
		return false;
	}

	if (!socket_bind_and_listen(s, server_info, ACCEPT_QUEUE_LENGTH)) {
		printf("Error: %s\n", strerror(errno));
		return false;
	}

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
  	unsigned char response[SERVER_RESPONSE_SIZE] = "OK\0";
	data_sent = socket_send(client_s, response, SERVER_RESPONSE_SIZE);
	if(!data_sent) return false;
	return true;
}

bool receive_message(socket_t *s, char* buffer, int size) {
   	int received_bytes = 0;
 	memset(buffer, 0, size); 
 	received_bytes = 0;
 	// Leeme size bytes y storeamelos en buffer
 	socket_receive(s, buffer, size, &received_bytes);
 	if (received_bytes < 0) return false;
 	return true;
}

void decode_params(char *content_buffer, int message_id) {
	printf("* Id: 0x%08x\n", message_id);

	int position = 0;
	int size = get_param_size(content_buffer);
	char *param = malloc(size);

	position = dbus_decode_line(content_buffer, position, param, size);
	printf("* Destino: %s\n", param);

	size = get_param_size(&content_buffer[position]);
	position = dbus_decode_line(content_buffer, position, param, size);
	printf("* Ruta: %s\n", param);

	size = get_param_size(&content_buffer[position]);
	position = dbus_decode_line(content_buffer, position, param, size);
	printf("* Interfaz: %s\n", param);

	size = get_param_size(&content_buffer[position]);
	position = dbus_decode_line(content_buffer, position, param, size);
	printf("* Metodo: %s\n", param);

	free(param);
}

void decode_body(char *content_buffer, int header_length, int body_length) {
	// La resta es porque el content_buffer no contempla la metadata
	int position = header_length - METADATA_SIZE;
	int body = body_length;
	int size = 0;
	// char param[size];
	if (body > 0){
		printf("* Parametros:\n");
		int bytes_read = 0;
		while(bytes_read < body) {
			memcpy(&size, &content_buffer[position], sizeof(int));
			// the \0 addition
			size += 1;
			position += sizeof(int);
			char *param = malloc(size);
			memcpy(param, &content_buffer[position], size);
			position += size;			
			bytes_read += (size + sizeof(int));
			printf("    * %s\n", param);
			free(param);
		}
	}
	printf("\n");
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

	while(true) {
		char message_buffer[METADATA_SIZE];
		bool received = receive_message(&client_s, message_buffer, METADATA_SIZE);
		if (!received) break;

		int body_length;
		int header_length;
		int message_id;

		memcpy(&body_length, &message_buffer[BODY_SIZE_POSITION], 4);
		memcpy(&header_length, &message_buffer[HEADER_SIZE_POSITION], 4);
		memcpy(&message_id, &message_buffer[ID_POSITION], 4);

		int content_size = body_length + header_length - METADATA_SIZE;
		char *content_buffer = malloc(content_size);
		receive_message(&client_s, content_buffer, content_size);

		decode_params(content_buffer, message_id);
		decode_body(content_buffer, header_length, body_length);
		free(content_buffer);

		if (!send_response(&client_s)) return ERROR;
	}

	server_destroy(&s, &client_s);
	return SUCCESS;
}
