#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct file_reader_t {
	FILE *data;
} file_reader_t;


bool file_reader_create(file_reader_t *self, char *filename);

void file_reader_destroy(file_reader_t *self);

char *file_reader_get_line(file_reader_t *self);

int _get_input(char **data, int len, FILE *fd);

#endif // FILE_READER_H
