#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>

#define CONSOLE_PROMPT "> "
#define CONSOLE_EOL "\r\n"

typedef void console_command_function(const char *cmd);

struct console_command_entry {
    const char *name;
    console_command_function *func;
    const char *help;
};

// The following should be defined by the user

void console_write(const char *buf, unsigned len);
int console_getchar(void);

extern const struct console_command_entry commands[];

// Main user interface

void console_init(void);
void console_process(void);

void console_write_string(const char *buf);
int console_parameter_uint16_hex(const char *buf, unsigned n, uint16_t *val);



#endif
