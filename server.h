#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>


/*Seteamos la direccion local de nuestra interfaz mediante getaddrinfo*/
/*Devuelve false si hubo algun error. True en caso contrario.*/
bool set_local_address(struct addrinfo *hints, struct addrinfo **ptr, const char *service);

/*Recibe un service a partir del cual se creara una conexion*/
/*Las conexiones creadas son cerradas al final.*/ 
/*En caso de errores devuelve 1. 0 en caso contrario.*/ 
int create_server(const char *service);

#endif // SERVER_H
