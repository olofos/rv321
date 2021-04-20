#include <string.h>

#include "console.h"

#define CONSOLE_BUF_SIZE 62
#define CONSOLE_MAX_PARAMS 8

#define CONSOLE_FLAG_ESCAPE 0x01
#define CONSOLE_FLAG_CSI    0x02
#define CONSOLE_FLAG_CR     0x04
#define CONSOLE_FLAG_ECHO   0x08

struct console_state {
    unsigned write_index;
    char *param_start;
    unsigned char flags;
    char buf[CONSOLE_BUF_SIZE+2];
};

static struct console_state state;

static unsigned find_nth_separator(const char* buf, unsigned n)
{
    unsigned index;
    for(index = 0; index < state.write_index; index++) {
        if(buf[index] == ' ') {
            n -= 1;
            if(n == 0) {
                break;
            }
        }
    }
    return index;
}

static const struct console_command_entry *find_command(const char *name, size_t len)
{
    for(unsigned i = 0; commands[i].name; i++) {
        if((strncmp(name, commands[i].name, len) == 0) && (commands[i].name[len] == 0)) {
            return &commands[i];
        }
    }
    return 0;
}

static void console_reset(void)
{
    state.write_index = 0;
    memset(state.buf, 0, CONSOLE_BUF_SIZE+2);
}

void console_init(void)
{
    console_reset();
    console_write_string("Ready." CONSOLE_EOL);
    state.flags = CONSOLE_FLAG_ECHO;
    console_write_string(CONSOLE_PROMPT);
}

void console_set_echo(int echo)
{
    if(echo) {
        state.flags |= CONSOLE_FLAG_ECHO;
    } else {
        state.flags &= ~CONSOLE_FLAG_ECHO;
    }
}

static void console_echo_string(const char *s)
{
    if(state.flags & CONSOLE_FLAG_ECHO) {
        console_write(s, strlen(s));
    }
}

static void console_echo(char c)
{
    if(state.flags & CONSOLE_FLAG_ECHO) {
        console_write(&c, 1);
    }
}


static int is_hex_digit(char c)
{
    if((('0' <= c) && (c <= '9')) || (('A' <= c) && (c <= 'F')) || (('a' <= c) && (c <= 'f'))) {
        return 1;
    }
    return 0;
}


static uint32_t from_hex_digit(char c)
{
    if(('0' <= c) && (c <= '9')) {
        return c - '0';
    }
    if(('A' <= c) && (c <= 'F')) {
        return 0xA + (c - 'A');
    }
    if(('a' <= c) && (c <= 'f')) {
        return 0xA + (c - 'a');
    }
    return 0;
}

int console_parameter_uint16_hex(const char *buf, unsigned n, uint16_t *val)
{
    unsigned param_start = find_nth_separator(buf, n) + 1;
    unsigned param_end = find_nth_separator(buf, n + 1);

    if(!buf[param_start]) {
        return 0;
    }

    if(buf[param_start+1] && (buf[param_start] == '0') && ((buf[param_start+1] == 'x') || (buf[param_start+1] == 'X'))) {
        param_start += 2;
    }

    if((param_end <= param_start) || (param_end > param_start + 4)) {
        return 0;
    }

    uint16_t result = 0;
    for(unsigned i = param_start; i < param_end; i++) {
        if(!is_hex_digit(buf[i])) {
            return 0;
        }
        result = (result << 4) | from_hex_digit(buf[i]);
    }

    *val = result;
    return 1;
}

static void handle_command(void)
{
    if(strlen(state.buf) > 0) {
        unsigned command_end = find_nth_separator(state.buf, 1);

        // We always have an extra NUL at the end
        // so we don't need to check that
        // command_end < CONSOLE_BUF_SIZE
        state.param_start = &state.buf[command_end + 1];

        const struct console_command_entry *entry = find_command(state.buf, command_end);

        if(entry) {
            if(entry->func) {
                entry->func(state.buf);
            }
        } else {
            console_write(state.buf, command_end);
            console_write_string(": command not found" CONSOLE_EOL);
        }
    }
    console_reset();
    console_echo_string(CONSOLE_PROMPT);
}

static void tab_completion(void)
{
    if(find_nth_separator(state.buf, 1) != state.write_index) {
        console_echo('\a');
        return;
    }

    size_t len = strlen(state.buf);
    unsigned matches = 0;
    char common_tail[16] = "";

    for(const struct console_command_entry *entry = commands; entry->name; entry++) {
        if(strncmp(state.buf, entry->name, len) == 0) {
            if(matches++ == 0) {
                strcpy(common_tail, entry->name + len);
            } else {
                for(unsigned i = 0; common_tail[i]; i++) {
                    if(entry->name[len + i] != common_tail[i]) {
                        common_tail[i] = 0;
                        break;
                    }
                }
            }
        }
    }

    if(matches == 0) {
        console_echo('\a');
        return;
    }

    if(matches == 1) {
        for(const char *p = common_tail; *p; p++) {
            state.buf[state.write_index++] = *p;
        }
        console_echo_string(common_tail);
        return;
    }

    if(common_tail[0] != 0) {
        for(const char *p = common_tail; *p; p++) {
            state.buf[state.write_index++] = *p;
        }
        console_echo_string(common_tail);
        return;
    } else {
        console_write_string(CONSOLE_EOL);
        for(const struct console_command_entry *entry = commands; entry->name; entry++) {
            if(strncmp(state.buf, entry->name, len) == 0) {
                matches++;
                console_write_string(entry->name);
                console_write_string(CONSOLE_EOL);
            }
        }
        console_write_string(CONSOLE_PROMPT);
        console_echo_string(state.buf);
    }
}

void console_write_string(const char *str)
{
    console_write(str, strlen(str));
}

void console_process(void)
{
    int input = console_getchar();
    if(input < 0) {
        return;
    }

    char c = input;

    // Ignore escape sequences, eg arrow keys
    if(state.flags & CONSOLE_FLAG_ESCAPE) {
        state.flags &= ~CONSOLE_FLAG_ESCAPE;
        if(c == '[') {
            state.flags |= CONSOLE_FLAG_CSI;
            return;
        }
    } else if(state.flags & CONSOLE_FLAG_CSI) {
        if((0x40 <= c) && (c < 0x7F)) {
            state.flags &= ~CONSOLE_FLAG_CSI;
        }
        return;
    }

    // Ignore LF after CR
    if(state.flags & CONSOLE_FLAG_CR) {
        state.flags &= ~CONSOLE_FLAG_CR;
        if(c == '\n') {
            return;
        }
    }

    switch(c) {
    case '\r':
        state.flags |= CONSOLE_FLAG_CR;
        // Fall through
    case '\n':
        console_write(CONSOLE_EOL, 2);
        handle_command();
        break;

    case '\x08': // Backspace
    case '\x7F': // DEL
        if(state.write_index > 0) {
            state.buf[--state.write_index] = 0;
            console_echo_string("\b \b");
        }
        break;

    case '\x03': // ^C
        console_reset();
        console_write_string(CONSOLE_EOL CONSOLE_PROMPT);
        break;

    case '\e': // ESC
        state.flags |= CONSOLE_FLAG_ESCAPE;
        break;

    case '\t':
        tab_completion();
        break;
    case ' ':
        if((state.write_index > 0) && (state.write_index < CONSOLE_BUF_SIZE) && (state.buf[state.write_index-1] != ' ')) {
            state.buf[state.write_index++] = c;
            console_echo(c);
        }
        break;

    default:
        if((0x20 <= c) && (c < 0x7F)) { // Ignore control characters and non-ascii characters
            if(state.write_index < CONSOLE_BUF_SIZE) {
                state.buf[state.write_index++] = c;
                if(state.flags & CONSOLE_FLAG_ECHO) {
                    console_echo(c);
                }
            } else {
                console_echo('\a');
            }
        }
        break;
    }
}
