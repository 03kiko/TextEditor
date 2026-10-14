#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "rawMode.h"
#include "errorHandler.h"

static struct termios orig_termios; // original terminal settings

/* Disables raw mode, restore original terminal settings */
static void disableRawMode() {
    safe_tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/*
* Enables raw mode for terminal input.
* Also sets a timeout for read()
*/
void enableRawMode(void) {
    // Get the current terminal attributes
    safe_tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode); // Ensure raw mode is disabled on exit

    // Modify the terminal attributes to enable raw mode
    struct termios raw = orig_termios;
    // Disable input flags
    raw.c_iflag &= ~(IXON | ICRNL | INPCK | ISTRIP | BRKINT);
    raw.c_oflag &= ~(OPOST); // Disable output post-processing
    raw.c_cflag |= (CS8); // Sets the character size to 8 bits per byte.
    // Disable local flags (echoing, canonical, signals)
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); 
    // Minimum number of bytes of input before read() returns
    raw.c_cc[VMIN] = 0; 
    raw.c_cc[VTIME] = 1; // Timeout for read() (in deciseconds)

    safe_tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // Apply modified attributes
}

int getCursorPosition(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;
    
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }

    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // fallback by putting curor at the end and get cords
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        if (getCursorPosition(rows, cols) == -1)
            return -1;
    
        return 0;
    }

    *cols = ws.ws_col;
    *rows = ws.ws_row;

    return 0;
}