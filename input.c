#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "input.h"
#include "errorHandler.h"
#include "editor.h"

// mirrors what the Ctrl key does in the terminal: it strips bits 5 and 6 from
// whatever key you press in combination with Ctrl
#define CTRL_KEY(k) ((k) & 0x1f) 

enum editorKey {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  HOME_KEY,
  END_KEY,
  DEL_KEY,
  PAGE_UP,
  PAGE_DOWN
};


static void editorMoveCursor(int key) {
    struct erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
        case ARROW_LEFT:
            if (E.cx != 0)
                E.cx--;
            else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_RIGHT:
            if (row && E.cx < row->size) {
                E.cx++;
            } else if (row && E.cx == row->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
        case ARROW_UP:
            if (E.cy != 0)
                E.cy--;
            break;
        case ARROW_DOWN:
            if (E.cy < E.numrows)
                E.cy++;
            break;
    }

    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) {
        E.cx = rowlen;
    }
}


static int editorReadKey() {
    ssize_t nread;
    char c;

    while ((nread = safe_read(STDIN_FILENO, &c, 1)) != 1) {
        // continues until exactly one byte is received
    }

    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';
                if (seq[2] == '~'){
                    switch (seq[1])
                    {
                        case '1':
                            return HOME_KEY;
                        case '3':
                            return DEL_KEY;
                        case '4':
                            return END_KEY;
                        case '5':
                            return PAGE_UP;
                        case '6':
                            return PAGE_DOWN;
                        case '7':
                            return HOME_KEY;
                        case '8':
                            return END_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A':
                        return ARROW_UP;
                    case 'B':
                        return ARROW_DOWN;
                    case 'C':
                        return ARROW_RIGHT;
                    case 'D':
                        return ARROW_LEFT;
                    case 'H':
                        return HOME_KEY;
                    case 'F':
                        return END_KEY;
                }
            }

            return '\x1b';
        } else {
            return c;
        }
    }

    return c;
}

void editorProcessKeypress(void) {
    int c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            safe_write(STDOUT_FILENO, "\x1b[2J", 4);
            safe_write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        
        case HOME_KEY:
            E.cx= 0;
            break;
        case END_KEY:
            if (E.cy < E.numrows)
                E.cx = E.row[E.cy].size;
            break;
        
        case PAGE_UP:
        case PAGE_DOWN:
            {
                if (c == PAGE_UP) {
                    E.cy = E.rowoff;
                } else if (c == PAGE_DOWN) {
                    E.cy = E.rowoff + E.screenrows - 1;
                    if (E.cy > E.numrows)
                        E.cy = E.numrows;
                }

                int times = E.screenrows;
                while (times--)
                    editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
            break;
        
        case ARROW_UP:
        case ARROW_LEFT:
        case ARROW_DOWN:
        case ARROW_RIGHT:
            editorMoveCursor(c);
            break;
    
    }
}