#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "editor.h"
#include "errorHandler.h"
#include "fileio.h"


static void editorUpdateRow(struct erow *row) {
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t')
            tabs++;
    
    char *new_render = malloc(row->size + tabs * (KILO_TAB_STOP - 1) + 1);
    if (new_render == NULL)
        die("malloc");
    free(row->render);
    row->render = new_render;

    int i = 0;
    for (j = 0; j < row->size; j++) {
        if (row->chars[j] == '\t') {
            row->render[i++] = ' ';
            while (i % KILO_TAB_STOP != 0)
                row->render[i++] = ' ';
        } else {
            row->render[i++] = row->chars[j];
        }
    }
    row->render[i] = '\0';
    row->rsize = i;

}

static void editorAppendRow(char *s, size_t len) {
    struct erow *new_row = realloc(E.row, sizeof(struct erow) * (E.numrows + 1));

    if (new_row == NULL)
        die("realloc");
    E.row = new_row;

    int at = E.numrows;
    E.row[at].size = len;
    
    char *new_chars = malloc(len + 1);
    if (new_chars == NULL)
        die("malloc");

    E.row[at].chars = new_chars;
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    editorUpdateRow(&E.row[at]);

    E.numrows++;
}

void editorOpen(const char *filename) {
    char *new_filename = strdup(filename);
    if (new_filename == NULL)
        die ("strdup");
    free(E.filename);
    E.filename = new_filename;
    
    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");
    
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                                line[linelen - 1] == '\r')) {
            linelen--;
        }

        editorAppendRow(line, linelen);
    }
    
    if (ferror(fp))
        die("getline");

    free(line);
    fclose(fp);
}