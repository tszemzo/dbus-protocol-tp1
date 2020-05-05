#ifndef DBUS_H
#define DBUS_H
#include <stdint.h>

typedef struct {
	unsigned char header[300];
	uint32_t id;
} dbus_t;

/* */
void apply_dbus(char *buffer, int size);
unsigned char *parse_line(dbus_t *self, char *buffer, int size);
void dbus_create(dbus_t *self);
void _set_header(dbus_t *self);
void _parse_param(unsigned char *header, char *buffer, int offset, char param_type, char data_type);
int _find_begin_param(char *buffer);
int _align_header(unsigned char *header, int header_position);
void _write_metadata_param(unsigned char *header, int pos, int offset, char param_type, char data_type);

#endif // DBUS_H	