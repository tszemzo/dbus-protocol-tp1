#ifndef CLIENT_H
#define CLIENT_H

#include "socket.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


/*Seteamos la direccion remota de nuestro cliente mediante getaddrinfo*/
/*Con el service y el host recibido por parametro.*/
/*En caso de que algo falle devuelve false. True en caso contrario.*/
bool set_remote_address(struct addrinfo *hints, struct addrinfo **client_info,
const char *host, const char *service);

/*Recibe un host y un service con los cuales se crea una conexion.*/
/*Se leen datos de data pointer y se envian a un servidor. */
/*Luego, el socket es cerrado al final.*/
/*En caso de errores devuelve 1. 0 en caso contrario.*/ 
bool client_run(const char *hostname, const char *servicename, const char *data);

#endif // CLIENT_H
