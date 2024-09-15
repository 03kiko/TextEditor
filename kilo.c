#include <ctype.h>
#include <stdio.h>
#include "rawMode.h"
#include "errorHandler.h"

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        safe_read(STDIN_FILENO, &c, 1);
        
        if (iscntrl(c))
            printf("%d\r\n", c);
        else
            printf("%d ('%c')\r\n", c, c);

        if (c == 'q')
            break;
    }
    
    return 0;
}