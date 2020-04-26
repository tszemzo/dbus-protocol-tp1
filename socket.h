#ifndef SOCKET_H
#define SOCKET_H
#define _POSIX_C_SOURCE 200112L

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct socket_t {
	int fd;
} socket_t;

/* Creates the socket */
bool socket_create(socket_t *self, struct addrinfo *ptr);

/* Destructs the socket .*/
// void socket_close(socket_t *self);

// int socket_connect(socket_t *self, char* hostname, char* service);

// int socket_accept(socket_t *self, socket_t *accepted_socket);

// int socket_bind_and_listen(socket_t *self, char* service);

// void socket_shutdown(socket_t *self);

#endif // SOCKET_H
