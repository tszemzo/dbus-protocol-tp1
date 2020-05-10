#include "file_reader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define CHUNK_SIZE 32
#define DATA_SIZE_MULTIPLIER 2

bool file_reader_create(file_reader_t *self, char *filename) {
    if (!filename) {
        self->data = stdin;
    } else {
        self->data = fopen(filename, "r"); 
    }
    if (!self->data) {
        printf("Error: could not open the file");
        return false;
    }
    return true;
}

void file_reader_destroy(file_reader_t *self) {
    fclose(self->data);
}

char* file_reader_get_line(file_reader_t *self) {
    char *data = {0};
    
    int read_bytes = _get_input(&data, 0, self->data);
    if (read_bytes < 0) {
        if (data != NULL) free(data);
        return NULL;
    }
    return data;
}

int _get_input(char **data, int len, FILE *fd) {
    char read_line[CHUNK_SIZE];
    if (*data == NULL) {
        len = sizeof(read_line);
        *data = malloc(len);
        if (!data) {
            printf("Error creating data on heap.\n");
            return -1;
        }
    }
    (*data)[0] = '\0';

    while (fgets(read_line, sizeof(read_line), fd) != NULL) {
        if (len - strlen(*data) < sizeof(read_line)) {
            len *= DATA_SIZE_MULTIPLIER;
            if ((*data = realloc(*data, len)) == NULL) {
                printf("Error creating data on heap.\n");
                free(data);
                return -1;
            }
        }
        strncat(*data, read_line, sizeof(read_line));
        if ((*data)[strlen(*data) - 1] == '\n') {
            return strlen(*data);
        }
    }
    return -1;
}
