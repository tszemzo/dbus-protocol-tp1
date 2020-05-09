#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>


/*Seteamos la direccion local de nuestra interfaz mediante getaddrinfo*/
/*En caso de que algo falle devuelve false. True en caso contrario.*/
bool set_local_address(struct addrinfo *hints, struct addrinfo **server_info,
	const char *service);

/*Crea servidor a partir del socket y la informacion brindada por svr_info. */
/*Bindea y aplica listen sobre el socket para ser el "pasivo".*/
/*En caso de que algo falle devuelve false. True en caso contrario.*/
bool bind_and_listen(socket_t *s, struct addrinfo *server_info);

/* Aplica accept y crea nuevo socket. */
/*En caso de que algo falle devuelve false. True en caso contrario.*/
bool server_accept(socket_t *s, socket_t *client_s);

/* Envia el OK luego de recibir el mensaje del cliente */
/*En caso de que algo falle devuelve false. True en caso contrario.*/
bool send_response(socket_t *client_s);

/*Recibe un service a partir del cual se creara una conexion*/
/*Las conexiones creadas son cerradas al final.*/ 
/*En caso de errores devuelve 1. 0 en caso contrario.*/ 
bool server_run(const char *service);

/*Destruye los sockets asociados al servidor y cierra las conexiones.*/ 
void server_destroy(socket_t *s, socket_t *client_s);

/*Recibe un mensaje de size bytes utilizando el socket.*/ 
bool receive_message(socket_t *s, char* buffer, int size);

#endif // SERVER_H
