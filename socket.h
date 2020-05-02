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

/* Crea el socket con la addrinfo recibida en ptr*/
/* Devuelve verdadero en caso de exito, falso si no. */
bool socket_create(socket_t *self, struct addrinfo *ptr);

/* Destruye el socket. */
void socket_destroy(socket_t *self);

/* Causa el cierre de la conexion full-duplex asociada al socket */
void socket_shutdown(socket_t *self);

/*Establece una conexion a la maquina remota.*/
/*Recibe el ptr que tiene la info de la address a conectarse.*/
/*Devuelve verdadero si se conecto, falso si no. */
bool socket_connect(socket_t *self, struct addrinfo *ptr);

/* Realiza el binding con la addrinfo recibida en ptr. */
/* Tambien setea el socket como uno pasivo gracias a listen */
/* Esto significa un socket que va a aceptar conexiones entrantes */
/* Tambien recibe el largo de la cola de aceptacion/espera */
/* Devuelve verdadero en caso de exito, falso si no. */
bool socket_bind_and_listen(socket_t *self, struct addrinfo *ptr, int accept_queue_length);

/* Extrae la primera conexion de la cola de conexiones pendientes de nuestro socket pasivo */
/* Crea un nuevo socket conectado y devuelve su fd que referencia a ese socket */
/* Devuelve verdadero en caso de exito, falso si no. */
bool socket_accept(socket_t *self, socket_t *accepted_socket);

/* Recibe data de la conexion establecida con el socket. */
/* Luego, la almacena en el data_buffer. */
/* Devuelve verdadero en caso de haber recibido todo la data, falso si no. */
bool socket_receive(socket_t *self, char* buffer, size_t size, int *received_bytes);

/* Envia lo contenido en data_buffer con largo size */
/* Devuelve verdadero en caso de exito */
/* Falso si hubo algun error con el socket o se cerro la direccion remota */
bool socket_send(socket_t *self, char* data_buffer, int size);

#endif // SOCKET_H
