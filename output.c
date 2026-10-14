#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "output.h"
#include "editor.h"
#include "appendBuffer.h"
#include "errorHandler.h"

#define KILO_VERSION "0.0.1"

static void editorDrawRows(abuf *ab) {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        if (y == E.screenrows / 3) {
            char welcome[80];
            int welcomelen = snprintf(welcome, sizeof(welcome),
                "Kilo editor -- version %s", KILO_VERSION);
            
            if (welcomelen > E.screencols)
                welcomelen = E.screencols;

            int padding = (E.screencols - welcomelen) / 2;
            if (padding) {
                abAppend(ab, "~", 1);
                padding--;
            }

            while (padding--) {
                abAppend(ab, " ", 1);
            }

            abAppend(ab, welcome, welcomelen);
        } else {
        abAppend(ab, "~", 1);
        }

        //clear 1 line at a time (instead of entire screen; more optimal)
        abAppend(ab, "\x1b[K", 3);
        if (y < E.screenrows - 1) {
            abAppend(ab, "\r\n", 2);
        }
    }
}

void editorRefreshScreen(void) {
    // '\x1b' = escape character (27)

    abuf ab = ABUF_INIT;
    
    // hide the cursor
    abAppend(&ab, "\x1b[?25l", 6);

    // Move cursor to the top-left corner
    abAppend(&ab, "\x1b[H", 3);

    editorDrawRows(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
    abAppend(&ab, buf, strlen(buf));

    // show cursor again
    abAppend(&ab, "\x1b[?25h", 6);

    safe_write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}