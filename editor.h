#ifndef _EDITOR_H
#define _EDITOR_H

struct editorConfig {
    int cx, cy;
    int screenrows;
    int screencols;
};

extern struct editorConfig E;

void initEditor(void);

#endif