#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "input.h"
#include "editor.h"
#include "errorHandler.h"
#include "output.h"
#include "fileio.h"

static char *editorRowsToString(int *buflen) {
    int totlen = 0;
    int i;

    for (i = 0; i < E.numrows; i++) {
        totlen += E.row[i].size + 1;
    }
    *buflen = totlen;

    char *buf = malloc((size_t)(totlen + 1));
    if (buf == NULL)
        die("malloc");
    char *p = buf;
    for (i = 0; i < E.numrows; i++) {
        memcpy(p, E.row[i].chars, E.row[i].size);
        p += E.row[i].size;
        *p = '\n';
        p++;
    }

    return buf;
}

void editorUpdateRow(struct erow *row) {
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

void editorInsertRow(int at, char *s, size_t len) {
    if (at < 0 || at > E.numrows)
        return;

    struct erow *new_row = realloc(E.row, sizeof(struct erow) * (E.numrows + 1));

    if (new_row == NULL)
        die("realloc");
    E.row = new_row;

    memmove(&E.row[at + 1], &E.row[at], sizeof(struct erow) * (E.numrows - at));

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
    E.dirty++;
}

static void editorFreeRow(struct erow *row) {
    free(row->render);
    free(row->chars);
}

void editorDelRow(int at) {
    if (at < 0 || at >= E.numrows)
        return;
    editorFreeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at+1], sizeof(struct erow) * (E.numrows - at - 1));
    E.numrows--;
    E.dirty++;
}

void editorRowInsertChar(struct erow *row, int at, int c) {
    if (at < 0 || at > row->size)
        at = row->size;
    char *new_chars = realloc(row->chars, row->size + 2);
    if (new_chars == NULL)
        die("realloc");
    row->chars = new_chars;
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowAppendString(struct erow *row, char *s, size_t len) {
    char *new_chars = realloc(row->chars, row->size + len + 1);
    if (new_chars == NULL)
        die("realloc");
    row->chars = new_chars;
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowDelChar(struct erow *row, int at) {
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editorUpdateRow(row);
    E.dirty++;
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

        editorInsertRow(E.numrows, line, linelen);
    }
    
    if (ferror(fp))
        die("getline");

    free(line);
    fclose(fp);
    E.dirty = 0;
}

void editorSave(void) {
    if (E.filename == NULL) {
        E.filename = editorPrompt("Save as: %s (ESC to cancel)");
        if (E.filename == NULL) {
            editorSetStatusMessage("Save aborted");
            return;
        }
    }

    int len;
    char *buf = editorRowsToString(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1) {
            safe_write(fd, buf, len);
            if (close(fd) == -1)
                die("close");
            free(buf);
            E.dirty = 0;
            editorSetStatusMessage("%d bytes written to disk", len);
            return;
        }
        if (close(fd) == -1)
            die("close");
    }

    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}
