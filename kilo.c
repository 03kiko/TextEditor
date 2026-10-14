#include "rawMode.h"
#include "editor.h"
#include "input.h"
#include "output.h"

int main(void) {
    enableRawMode();
    initEditor();

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    
    return 0;
}