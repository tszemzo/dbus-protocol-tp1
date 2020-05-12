#ifndef DBUS_H
#define DBUS_H
#include <stdint.h>

typedef struct {
	unsigned char header[500];
	uint32_t id;
	int header_length;
	int body_length;
} dbus_t;

/* */
unsigned char *dbus_encode_line(dbus_t *self, char *buffer, int size);
int dbus_decode_line(char* content_buffer, int position, char *param, int size);
void dbus_create(dbus_t *self);
void _set_header(dbus_t *self);
void _parse_param(unsigned char *header, char *buffer, int offset, 
	char param_type, char data_type);
void _parse_firm(unsigned char *header, int offset, char param_type,
	char data_type);
int _next_word_offset(char *content_buffer);
int _parse_body(unsigned char *header, char *buffer);
int _find_begin_param(char *buffer);
int _find_firm_params(char *buffer);
int _align_header(unsigned char *header, int header_position);
void _add_lengths(dbus_t *self, int header_length, int body_length);
void _write_metadata_param(unsigned char *header, int pos, int offset, 
	char param_type, char data_type);
uint32_t dbus_body_length(dbus_t *self);
uint32_t dbus_header_length(dbus_t *self);
uint32_t dbus_id(dbus_t *self);
int get_param_start(char *dest_start, char* param, int size);
int get_param_size(char *dest_start);

#endif // DBUS_H
