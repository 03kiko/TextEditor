#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <sys/types.h>
#include <termios.h>

void die(const char* s);
ssize_t safe_read(int fd, void* buf, size_t count);
void safe_write(int fd, const void *buf, size_t count);
void safe_tcsetattr(int fd, int optional_actions,
                    const struct termios* termios_p);
void safe_tcgetattr(int fd, struct termios *termios_p);

#endif
