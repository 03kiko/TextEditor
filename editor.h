#ifndef EDITOR_H
#define EDITOR_H

#include <time.h>

#define KILO_TAB_STOP 8

struct erow {
    int size;
    char *chars;
    char *render;
    int rsize;
};
struct editorConfig {
    int cx, cy;
    int rx;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int numrows;
    struct erow *row;
    int dirty;
    char *filename;
    char statusmsg[80];
    time_t statusmsg_time;
};

extern struct editorConfig E;

void initEditor(void);

#endif