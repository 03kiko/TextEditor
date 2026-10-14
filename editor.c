#include "editor.h"
#include "errorHandler.h"
#include "rawMode.h"

struct editorConfig E;

void initEditor(void) {
    E.cx = 0;
    E.cy = 0;

    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindownSize");
}