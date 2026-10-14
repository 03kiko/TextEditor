#include <string.h>
#include <stdlib.h>

#include "appendBuffer.h"
#include "errorHandler.h"

void abAppend(abuf* ab, const char* s, int len) {
    if (len <= 0)
        return;

    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL)
        die("realloc");
    
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(abuf *ab) {
    free(ab->b);
    ab->b = NULL;
    ab->len = 0;
}