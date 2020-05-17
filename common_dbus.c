#include "dbus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define METADATA_LENGTH 8
#define METADATA_PARAM_LENGTH 4
#define FIRM_METADATA_LENGTH 5
#define START_OF_PARAMS 16
#define ALIGNMENT 8

unsigned char *dbus_encode_line(dbus_t *self, char* buffer, int size) {
	_set_header(self);
	// destination
	char param_type = '6';
	char data_type = 's';
	int current_position = 0;
	int offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[START_OF_PARAMS], &buffer[current_position], 
		offset, param_type, data_type);
	int header_position = METADATA_LENGTH + START_OF_PARAMS + offset + 1;
	header_position = _align_header(&self->header[header_position], 
		header_position);
	// route
	param_type = '1';
	data_type = 'o';
	current_position = current_position + offset + 1;
	offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[header_position], &buffer[current_position], 
		offset, param_type, data_type);
	header_position = header_position + METADATA_LENGTH + offset + 1;
	header_position = _align_header(&self->header[header_position], 
		header_position);
	// interface
	param_type = '2';
	data_type = 's';
	current_position = current_position + offset + 1;
	offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[header_position], &buffer[current_position], 
		offset, param_type, data_type);
	header_position = header_position + METADATA_LENGTH + offset + 1;
	header_position = _align_header(&self->header[header_position], 
		header_position);
	// method
	param_type = '3';
	data_type = 's';
	current_position = current_position + offset + 1;
	offset = _find_begin_param(&buffer[current_position]);
	_parse_param(&self->header[header_position], &buffer[current_position], 
		offset, param_type, data_type);
	header_position = header_position + METADATA_LENGTH + offset + 1;
	header_position = _align_header(&self->header[header_position], 
		header_position);
	// firma
	param_type = '9';
	data_type = 'g';
	current_position = current_position + offset + 1;
	// saving the firm position for the body
	int firm_position = current_position;
	offset = _find_firm_params(&buffer[current_position]);
	_parse_firm(&self->header[header_position], offset, param_type, data_type);
	header_position = header_position + FIRM_METADATA_LENGTH + offset;
	header_position = _align_header(&self->header[header_position], 
		header_position);

	self->header_length = header_position;
	self->body_length = _parse_body(&self->header[header_position], 
		&buffer[firm_position]);

	_add_lengths(self, self->header_length, self->body_length);

	self->id += 1;
	return self->header;
}

int dbus_decode_line(char* content_buffer, int position, char* param, 
	int size) {
	int length = position;
	int offset = _next_word_offset(&content_buffer[length]);
	length += offset + METADATA_LENGTH;
	memcpy(param, &content_buffer[length], size);
	length += size;
	return length;
}

void _add_lengths(dbus_t *self, int header_length, int body_length) {
    memcpy(&self->header[4], &body_length, 4);
    memcpy(&self->header[12], &header_length, 4);
}

int _find_begin_param(char *buffer) {
	int param_size = 0;
	bool found = false;
	while (!found) {
		if (buffer[param_size] == ' ' || buffer[param_size] == '(' || 
			buffer[param_size] == '\n'){
			found = true;
			break;
		}
		param_size++;
	}
	return param_size;
}

int _find_firm_params(char *buffer) {
	int param_size = 0;
	int params_count = 0;
	while (buffer[param_size] != ')') {
		if (buffer[param_size] == ','){
			params_count += 1;
		}
		param_size++;
	}
	if (param_size > 0){
		params_count++;	
	}
	return params_count;
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

void _parse_firm(unsigned char *header, int offset, char param_type, 
	char data_type) {
	int pos = 0;
	_write_metadata_param(header, pos, offset, param_type, data_type);
	pos += FIRM_METADATA_LENGTH;
	for (int i = 0; i < offset; i++) {
		header[pos] = 's';
		pos++;
	}
	header[pos] = '\0';
}

int _parse_body(unsigned char *header, char *buffer) {
	int body_size = 0;
	int pos = 0;
	int word_size = 0;
	while (buffer[body_size] != ')') {
		if (buffer[body_size] == ','){
			memcpy(&header[pos], &word_size, 4);
			pos += 4;
			memcpy(&header[pos], &buffer[body_size - word_size], word_size);
			pos += word_size;
			header[pos] = '\0';
			pos += 1;
			word_size = 0;
			body_size++;
		} else {
			word_size++;
			body_size++;
		}
	}
	if (body_size > 0) {
		memcpy(&header[pos], &word_size, 4);
		pos += 4;
		memcpy(&header[pos], &buffer[body_size - word_size], word_size);
		pos += word_size;
		header[pos] = '\0';
		pos += 1;
	}
	return pos;
}

void _parse_param(unsigned char *header, char *buffer, int offset, 
	char param_type, char data_type) {
	char *param = malloc(offset);
	memcpy(param, buffer, offset);
	int pos = 0;
	_write_metadata_param(header, pos, offset, param_type, data_type);
	pos += METADATA_LENGTH;
	memcpy(&header[pos], param, offset);
	pos += offset;
	header[pos] = '\0';
	free(param);
}

void _write_metadata_param(unsigned char *header, int pos, int offset, 
	char param_type, char data_type) {
	header[pos] = param_type;
	pos += 1;
	header[pos] = '1';
	pos += 1;
	header[pos] = data_type;
	pos += 1;
	header[pos] = '\0';
	pos += 1;
	char firm_param_type = '9';
	if (param_type == firm_param_type){
		memcpy(&header[pos], &offset, 1);
	} else {
		memcpy(&header[pos], &offset, 4);
	}
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
	memset(self->header, 0, 500);
}

int get_param_size(char *dest_start){
	int pos = 0;
	while (dest_start[pos] == '\0') {
		pos++;
	}
	pos += METADATA_PARAM_LENGTH;
	int last_cero = 1;
	int size = dest_start[pos] + last_cero;
	return size;
}

int _next_word_offset(char *content_buffer){
	int pos = 0;
	while (content_buffer[pos] == '\0') {
		pos++;
	}
	return pos;
}

uint32_t dbus_body_length(dbus_t *self){
	return self->body_length;
}

uint32_t dbus_header_length(dbus_t *self){
	return self->header_length;
}

uint32_t dbus_id(dbus_t *self){
	return self->id;
}

