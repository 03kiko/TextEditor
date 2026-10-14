#ifndef APPEND_BUFFER_H
#define APPEND_BUFFER_H

#include <stddef.h>

#define ABUF_INIT {NULL, 0}

typedef struct {
    char* b;
    int len;
} abuf;

void abAppend(abuf* ab, const char* s, int len);
void abFree(abuf* ab);

#endif