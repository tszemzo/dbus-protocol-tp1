#include "dbus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define DESTINY_TYPE "6"
#define ROUTE_TYPE "1"
#define INTERFACE_TYPE "2"
#define METHOD_TYPE "3"
#define FIRM_TYPE "9"
#define METADATA_LENGTH 8
#define START_OF_PARAMS 16
#define ALIGNMENT 8

unsigned char *parse_line(dbus_t *self, char* buffer, int size) {

	_set_header(self);
	// destination
	char param_type = '6';
	char data_type = 's';
	int current_position = 0;
	int offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[START_OF_PARAMS], &buffer[current_position], offset, param_type, data_type);
	int header_position = METADATA_LENGTH + START_OF_PARAMS + offset;
	header_position = _align_header(&self->header[header_position], header_position);
	// route
	param_type = '1';
	data_type = 'o';
	current_position = current_position + offset + 1;
	offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[header_position], &buffer[current_position], offset, param_type, data_type);
	header_position = header_position + METADATA_LENGTH + offset;
	header_position = _align_header(&self->header[header_position], header_position);
	// interface
	param_type = '2';
	data_type = 's';
	current_position = current_position + offset + 1;
	offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[header_position], &buffer[current_position], offset, param_type, data_type);
	header_position = header_position + METADATA_LENGTH + offset;
	header_position = _align_header(&self->header[header_position], header_position);
	// method
	param_type = '3';
	data_type = 's';
	current_position = current_position + offset + 1;
	offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[header_position], &buffer[current_position], offset, param_type, data_type);
	header_position = header_position + METADATA_LENGTH + offset;
	header_position = _align_header(&self->header[header_position], header_position);
	printf("alineado?? %d\n", header_position);

	return self->header;
}

int _find_begin_param(char *buffer) {
	int param_size = 0;
	bool found = false;
	while (!found) {
		if (buffer[param_size] == ' ' || buffer[param_size] == '(' || buffer[param_size] == '\n'){
			found = true;
			break;
		}
		param_size++;
	}
	printf("Param Size: %d\n", param_size);
	return param_size;
}

int _align_header(unsigned char *header, int header_position) {
	int pos = 0;
	while(header_position % ALIGNMENT != 0) {
		header[pos] = '\0';
		header_position++;
		pos++;
	}
	return header_position;
}

void _parse_param(unsigned char *header, char *buffer, int offset, char param_type, char data_type) {
	char param[offset];
	memcpy(param, buffer, offset);
	printf("param: %s\n", param);
	printf("param_type: %c\n", param_type);
	int pos = 0;
	_write_metadata_param(header, pos, offset, param_type, data_type);
	pos += METADATA_LENGTH;
	memcpy(&header[pos], &param, offset);
	pos += offset;
	header[pos] = '\0';
	printf("post header: %s\n", header);
}

void _write_metadata_param(unsigned char *header, int pos, int offset, char param_type, char data_type) {
	header[pos] = param_type;
	pos += 1;
	header[pos] = '1';
	pos += 1;
	header[pos] = data_type;
	pos += 1;
	header[pos] = '\0';
	pos += 1;
	memcpy(&header[pos], &offset, 4);
}

void _set_header(dbus_t *self) {
	self->header[0] = 'l';
	self->header[1] = '1';
	self->header[2] = '0';
	self->header[3] = '1';
	uint32_t body_length = 0;
	uint32_t header_length = 0;
    memcpy(&self->header[4], &body_length, 4);
	memcpy(&self->header[8], &self->id, 4); 
    memcpy(&self->header[12], &header_length, 4);
}

void dbus_create(dbus_t *self) {
	(self->id) = 1;
	memset(self->header, 1, 300);

}
