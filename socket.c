#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool socket_create(socket_t *self, struct addrinfo *ptr) {
	self->fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (self->fd == -1) return false;
	return true;
}

bool socket_bind_and_listen(socket_t *self, struct addrinfo *ptr, int accept_queue_length){
	// Así se puede reutilizar el puerto sin esperar timeout
    int val = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	int binding = bind(self->fd, ptr->ai_addr, ptr->ai_addrlen);
	freeaddrinfo(ptr);
	printf("Binding result...%d\n", binding);

	if (binding == -1) {
      close(self->fd);
      return false;
    }

    int listening = listen(self->fd, accept_queue_length);
    printf("Listening result...%d\n", listening);

    if (listening == -1) {
    	close(self->fd);
    	return false;
    }

    return true;
}

bool socket_receive(socket_t *self, char* buffer, size_t buffer_size,
	int *received_bytes, int *accum_bytes) {

	while (*accum_bytes < buffer_size) {
  	 	*received_bytes = recv(self->fd, &buffer[*accum_bytes], buffer_size - *accum_bytes, MSG_NOSIGNAL);
  	 		if (*received_bytes == 0) {
  	 			*received_bytes = -1;
  	 			break;
  	 		}
  	 		if (*received_bytes < 0) {
				printf("Error: %s\n", strerror(errno));
				return false;
			}
			*accum_bytes += *received_bytes;
	}
	return true;
}

bool socket_accept(socket_t *self, socket_t *new_socket) {
	new_socket->fd = accept(self->fd, NULL, NULL);
	if ( new_socket->fd == -1) return false;

	printf("Accepting socket fd...%d\n", new_socket->fd);
	return true;
}
