#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

#include "errorHandler.h"

/*
 * Prints error message and exits
 * @param s The string to output with perror before exiting.
 */
void die(const char* s) {
    safe_write(STDOUT_FILENO, "\x1b[2J", 4);
    safe_write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

/* Wrapper around read() with error handling */
ssize_t safe_read(int fd, void* buf, size_t count) {
    ssize_t result;
    do
    {
        result = read(fd, buf, count);
    } while (result != -1 && errno == EINTR);
    
    if (result == -1 && errno != EAGAIN)
        die("read");

    return result;
}

/* Wrapper around write() with error handling */
void safe_write(int fd, const void* buf, size_t count) {
    const char *p = buf;
    size_t written = 0;

    while (written < count) {
        ssize_t n = write(fd, p + written, count - written);

        if (n == -1) {
            if (errno == EINTR)
                continue;
            
            die("write");
        }

        if (n == 0) {
            errno = EIO;
            die("write");
        }

        written += (size_t)n;
    }
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