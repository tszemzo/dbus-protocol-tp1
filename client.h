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

/*Crea el cliente con la informacion de la address remota*/
/*Luego, intenta conectarlo a la misma.*/
/*Devuelve verdadero si se pudo, false en caso contrario liberando recursos.*/
bool client_connect(socket_t *s, struct addrinfo *result);

/*Envia la data a traves del socket conectado a una direccion remota.*/
/*Devuelve verdadero si se pudo, false si hubo un error en el envio.*/
bool send_data(socket_t *s, FILE *data);

/*Recibe un host y un service con los cuales se crea una conexion.*/
/*Se leen datos de data y se envian a un servidor. */
/*Luego, el socket es cerrado al final.*/
/*En caso de errores devuelve 1. 0 en caso contrario.*/ 
bool client_run(const char *host, const char *service, char *data);

/*Destruye el socket asociado al client y cierra la conexion.*/ 
void client_destroy(socket_t *s);

#endif // CLIENT_H
