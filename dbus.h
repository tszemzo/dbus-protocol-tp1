#ifndef DBUS_H
#define DBUS_H
#include <stdint.h>

typedef struct {
	unsigned char header[500];
	uint32_t id;
	int header_length;
	int body_length;
} dbus_t;

/*Encargado de codificar una linea al protocolo dbus. La utiliza el cliente*/ 
unsigned char *dbus_encode_line(dbus_t *self, char *buffer, int size);

/*Encargado de decodificar una linea al protocolo dbus. La utiliza el server*/ 
int dbus_decode_line(char* content_buffer, int position, char *param, int size);

/*Crea el dbus seteando el id en 1.*/ 
void dbus_create(dbus_t *self);

/*Metodo privado. Agrega la metadata del header del mensaje.*/ 
void _set_header_metadata(dbus_t *self);

/*Metodo privado. Encargado de parsear un parametro dado en dbus.*/ 
void _parse_param(unsigned char *header, char *buffer, int offset, 
	char param_type, char data_type);

/*Metodo privado. Encargado de parsear la firma en dbus.*/ 
void _parse_firm(unsigned char *header, int offset, char param_type,
	char data_type);

/*Metodo privado. Devuelve el offset hasta la proxima palabra dentro del msg.*/
int _next_word_offset(char *content_buffer);

/*Metodo privado. Parsea el body del mensaje.*/
int _parse_body(unsigned char *header, char *buffer);

/*Metodo privado. Calcula el offset para encontrar el siguiente param.*/
int _find_begin_param(char *buffer);

/*Metodo privado. Calcula el offset para encontrar el firm param.*/
int _find_firm_params(char *buffer);

/*Metodo privado. Calcula la posicion hasta alinear a 8 bytes el mensaje.*/
int _align_header(unsigned char *header, int header_position);

/*Metodo privado. Agrega los largos del body y el header al mensaje.*/
void _add_lengths(dbus_t *self, int header_length, int body_length);

/*Metodo privado. Escribe la metadata de cada parametro en parse_param.*/
void _write_metadata_param(unsigned char *header, int pos, int offset, 
	char param_type, char data_type);

/*Devuelve el largo del body del mensaje.*/
uint32_t dbus_body_length(dbus_t *self);

/*Devuelve el largo del header del mensaje.*/
uint32_t dbus_header_length(dbus_t *self);

/*Devuelve el id incremental de mensajes enviados por el protocolo.*/
uint32_t dbus_id(dbus_t *self);

int get_param_size(char *dest_start);

#endif // DBUS_H
