#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool socket_create(socket_t *self, struct addrinfo *ptr) {
	self->fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (self->fd == -1) return false;
	return true;
}
