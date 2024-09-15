#include <stdlib.h>
#include "rawMode.h"
#include "errorHandler.h"

static struct termios orig_termios; // original terminal settings

/* Disables raw mode, restore original terminal settings */
void disableRawMode() {
    safe_tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/*
* Enables raw mode for terminal input.
* Also sets a timeout for read()
*/
void enableRawMode() {
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