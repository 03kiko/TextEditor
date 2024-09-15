#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "errorHandler.h"


/*
 * Prints error message and exits
 * @param s The string to output with perror before exiting.
 */
void die(const char* s) {
    perror(s);
    exit(1);
}

/* Wrapper around read() with error handling */
ssize_t safe_read(int fd, void* buf, size_t count) {
    ssize_t result;
    if ((result = read(fd, buf, count)) == -1 && errno != EAGAIN)
        die("read");

    return result;
}

/* Wrapper around tcsetattr() with error handling */
void safe_tcsetattr(int fd, int optional_actions,
                    const struct termios* termios_p) {
    if (tcsetattr(fd, optional_actions, termios_p) == -1)
        die("tcsetattr");  // Exit on error
}

/* Wrapper around tcgetattr() with error handling */
void safe_tcgetattr(int fd, struct termios *termios_p) {
    if (tcgetattr(fd, termios_p) == -1)
        die("tcgetattr");  // Exit on error
}