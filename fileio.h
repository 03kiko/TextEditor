#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>

#include "editor.h"

void editorUpdateRow(struct erow *row);
void editorInsertRow(int at, char *s, size_t len);
void editorDelRow(int at);
void editorRowInsertChar(struct erow *row, int at, int c);
void editorRowAppendString(struct erow *row, char *s, size_t len);
void editorRowDelChar(struct erow *row, int at);
void editorOpen(const char *filename);
void editorSave(void);

#endif