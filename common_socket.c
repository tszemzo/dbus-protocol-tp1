#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

bool socket_create(socket_t *self, struct addrinfo *ptr) {
	self->fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (self->fd == -1) return false;
	return true;
}

void socket_destroy(socket_t *self) {
	close(self->fd);
}

void socket_shutdown(socket_t *self) { 
	shutdown(self->fd, SHUT_RDWR); 
}

bool socket_connect(socket_t *self, struct addrinfo *ptr) {
	int connection = connect(self->fd, ptr->ai_addr, ptr->ai_addrlen);
	if (connection == -1) return false;
	return true;
}

bool socket_bind_and_listen(socket_t *self, struct addrinfo *ptr, 
	int accept_queue_length) {
	// Así se puede reutilizar el puerto sin esperar timeout
    int val = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	int binding = bind(self->fd, ptr->ai_addr, ptr->ai_addrlen);
	freeaddrinfo(ptr);

	if (binding == -1) {
      close(self->fd);
      return false;
    }

    int listening = listen(self->fd, accept_queue_length);

    if (listening == -1) {
    	close(self->fd);
    	return false;
    }

    return true;
}

bool socket_receive(socket_t *self, char* buffer, size_t size, 
	int *received_bytes) {
	int accum_bytes = 0;
	while (accum_bytes < size) {
  	 	*received_bytes = recv(self->fd, &buffer[accum_bytes], 
  	 		size - accum_bytes, MSG_NOSIGNAL);
  	 	if (*received_bytes == 0) {
  	 		*received_bytes = -1;
  	 		break;
  	 	}
  	 	if (*received_bytes < 0) {
			printf("Error: %s\n", strerror(errno));
			return false;
		}
		accum_bytes += *received_bytes;
	}
	return true;
}

bool socket_send(socket_t *self, unsigned char* data_buffer, int size) {
	bool error_sending = false;
	int accum_bytes = 0;
	int sent_bytes;
	
	while (accum_bytes < size && !error_sending) {
		int current_position = size - accum_bytes;
      	sent_bytes = send(self->fd, &data_buffer[accum_bytes], 
      		current_position, MSG_NOSIGNAL);

      	if (sent_bytes < 0) {
      		error_sending = true;
         	printf("Error: %s\n", strerror(errno));
      	} else if (sent_bytes == 0) { 
      		error_sending = true;
      		printf("Error: socket del servidor cerrado\n");
      	} else {
         	accum_bytes += sent_bytes;
      	}
	}
	if (error_sending) return false;
	return true;
}

bool socket_accept(socket_t *self, socket_t *new_socket) {
	new_socket->fd = accept(self->fd, NULL, NULL);
	if ( new_socket->fd == -1) return false;
	return true;
}
