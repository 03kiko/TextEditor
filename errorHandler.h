#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_

#include <unistd.h>
#include "rawMode.h"

ssize_t safe_read(int fd, void* buf, size_t count);
void safe_tcsetattr(int fd, int optional_actions,
                    const struct termios* termios_p);
void safe_tcgetattr(int fd, struct termios *termios_p);

#endif
