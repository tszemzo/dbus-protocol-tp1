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
#define ACCEPT_QUEUE_LENGTH 10
#define METADATA_SIZE 16
#define HEADER_SIZE_POSITION 12
#define BODY_SIZE_POSITION 4
#define ID_POSITION 8
#define SERVER_RESPONSE_SIZE 3
#define METADATA_CONTENT_SIZE 8

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

bool receive_message(socket_t *s, char* buffer, int size) {
   	int received_bytes = 0;
 	memset(buffer, 0, size); 
 	received_bytes = 0;
 	// Leeme size bytes y storeamelos en buffer
 	socket_receive(s, buffer, size, &received_bytes);
 	printf("RECEIVED %d\n", received_bytes);
 	if (received_bytes < 0) return false;
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

	while(true) {
		// This is going to be encapsulated
		char message_buffer[METADATA_SIZE];
		bool received = receive_message(&client_s, message_buffer, METADATA_SIZE);
		if (!received) {
			printf("Entro \n");
			break;
		}
		int body_length = message_buffer[BODY_SIZE_POSITION];
		int header_length = message_buffer[HEADER_SIZE_POSITION];
		int message_id = message_buffer[ID_POSITION];
		
		header_length = 160;

		// int body_length = m[4] + (m[5] << 8) + (m[6] << 16) + (m[7] << 24);
	 //    int message_id = m[8] + (m[9] << 8) + (m[10] << 16) + (m[11] << 24);
	 //    int header_length = m[12] + (m[13] << 8) + (m[14] << 16) + (m[15] << 24);

	    printf("body length %d\n", body_length);
		printf("header_length %d\n", header_length);
		printf("message_id %d\n", message_id);

		int content_size = body_length + header_length - METADATA_SIZE;
		char content_buffer[content_size];		
		receive_message(&client_s, content_buffer, content_size);

		printf("* Id: %04d\n", message_id);
		int size = get_param_size(content_buffer);
		int offset = 0;
		int position = offset + METADATA_CONTENT_SIZE;
		char param[size];
		memcpy(&param, &content_buffer[position], size);
		printf("* Destino: %s\n", param);
		position += size;

		size = get_param_size(&content_buffer[position]);
		offset = next_word_offset(&content_buffer[position]);
		position += offset + METADATA_CONTENT_SIZE;
		memcpy(&param, &content_buffer[position], size);
		printf("* Ruta: %s\n", param);
		position += size;

		size = get_param_size(&content_buffer[position]);
		offset = next_word_offset(&content_buffer[position]);
		position += offset + METADATA_CONTENT_SIZE;
		memcpy(&param, &content_buffer[position], size);
		printf("* Interfaz: %s\n", param);
		position += size;

		size = get_param_size(&content_buffer[position]);
		offset = next_word_offset(&content_buffer[position]);
		position += offset + METADATA_CONTENT_SIZE;
		memcpy(&param, &content_buffer[position], size);
		printf("* Metodo: %s\n", param);

		printf("* Parametros:\n");
		// La resta es porque el content_buffer no contempla la metadata
		position = header_length - METADATA_SIZE;
		if (body_length > 0){
			int bytes_read = 0;
			while(bytes_read < body_length) {
				memcpy(&size, &content_buffer[position], sizeof(int));
				// the \0 addition
				size += 1;
				position += sizeof(int);
				memcpy(&param, &content_buffer[position], size);
				position += size;			
				bytes_read += (size + sizeof(int));
				printf("    * %s\n", param);
			}
		}

		if (!send_response(&client_s)) return ERROR;
	}

	server_destroy(&s, &client_s);
	return SUCCESS;
}
