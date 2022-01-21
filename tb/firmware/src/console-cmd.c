#include <stdio.h>
#include <string.h>

#include "console.h"
#include "mcp2317.h"

void cmd_read_io(const char *cmd)
{
    uint16_t chip;
    if(console_parameter_uint16_hex(cmd, 1, &chip)) {
        if(chip > 4) {
            console_write_string("Chip shoud be in the range [0,4]\r\n");
            return;
        }

        uint16_t val = mcp2317_read(chip, MCP2317_REG_GPIO);

        printf("Read %u: %04X\r\n", chip, val);
    } else {
        for(int i = 0; i < 5; i++) {
            uint16_t val = mcp2317_read(i, MCP2317_REG_GPIO);

            if(i > 0) {
                printf(" ");
            }

            printf("%04X", val);
        }
        printf("\r\n");
    }
}

void cmd_read_iodir(const char *cmd)
{
    uint16_t chip;
    if(console_parameter_uint16_hex(cmd, 1, &chip)) {
        if(chip > 4) {
            console_write_string("Chip shoud be in the range [0,4]\r\n");
            return;
        }

        uint16_t val = mcp2317_read(chip, MCP2317_REG_IODIR);

        printf("Read %u: %04X\r\n", chip, val);
    } else {
        for(int i = 0; i < 5; i++) {
            uint16_t val = mcp2317_read(i, MCP2317_REG_IODIR);

            if(i > 0) {
                printf(" ");
            }

            printf("%04X", val);
        }
        printf("\r\n");
    }
}

void cmd_read_pullup(const char *cmd)
{
    uint16_t chip;
    if(console_parameter_uint16_hex(cmd, 1, &chip)) {
        if(chip > 4) {
            console_write_string("Chip shoud be in the range [0,4]\r\n");
            return;
        }

        uint16_t val = mcp2317_read(chip, MCP2317_REG_GPPU);

        printf("Read %u: %04X\r\n", chip, val);
    } else {
        for(int i = 0; i < 5; i++) {
            uint16_t val = mcp2317_read(i, MCP2317_REG_GPPU);

            if(i > 0) {
                printf(" ");
            }

            printf("%04X", val);
        }
        printf("\r\n");
    }
}



void cmd_write_io(const char *cmd)
{
    uint16_t values[5];

    for(unsigned chip = 0; chip < 5; chip++) {
        if(!console_parameter_uint16_hex(cmd, chip + 1, &values[chip])) {
            console_write_string("Usage: write-io val1 ... val5\r\n");
            return;
        }
    }

    for(unsigned chip = 0; chip < 5; chip++) {
        mcp2317_write(chip, MCP2317_REG_OLAT, values[chip]);
    }
}

void cmd_write_iodir(const char *cmd)
{
    uint16_t values[5];

    for(unsigned chip = 0; chip < 5; chip++) {
        if(!console_parameter_uint16_hex(cmd, chip + 1, &values[chip])) {
            console_write_string("Usage: write-iodir val1 ... val5\r\n");
            return;
        }
    }

    for(unsigned chip = 0; chip < 5; chip++) {
        mcp2317_write(chip, MCP2317_REG_IODIR, values[chip]);
    }
}

void cmd_write_pullup(const char *cmd)
{
    uint16_t values[5];

    for(unsigned chip = 0; chip < 5; chip++) {
        if(!console_parameter_uint16_hex(cmd, chip + 1, &values[chip])) {
            console_write_string("Usage: write-pullup val1 ... val5\r\n");
            return;
        }
    }

    for(unsigned chip = 0; chip < 5; chip++) {
        mcp2317_write(chip, MCP2317_REG_GPPU, values[chip]);
    }
}

void pwm_set_anim(int);
void cmd_set_anim(const char *cmd)
{
    uint16_t anim;
    if(!console_parameter_uint16_hex(cmd, 1, &anim)) {
        console_write_string("Usage: set-anim anim\r\n");
        return;
    }

    pwm_set_anim(anim);
}

void cmd_echo(const char *cmd)
{
    if(strcmp(cmd, "echo on") == 0) {
        console_set_echo(1);
    } else if(strcmp(cmd, "echo off") == 0) {
        console_set_echo(0);
    } else {
        console_write_string("Usage console on|off\r\n");
    }
}

void cmd_version(const char __attribute__((__unused__)) *cmd)
{
    console_write_string("Version 0.1" CONSOLE_EOL);
}

void cmd_test(const char __attribute__((__unused__)) *cmd)
{
    console_write_string("This is a simple test" CONSOLE_EOL "It ignores any parameters" CONSOLE_EOL);
}

void cmd_test_u16(const char *cmd)
{
    console_write_string("This is test-u16" CONSOLE_EOL);

    for(unsigned n = 1; n < 5; n++) {
        uint16_t val;
        if(!console_parameter_uint16_hex(cmd, n, &val)) {
            break;
        }
        char s[32];
        sprintf(s, "Parameter %d: %04X" CONSOLE_EOL, n, val);
        console_write_string(s);
    }
}

void cmd_help(const char __attribute__((__unused__)) *cmd)
{
    const struct console_command_entry *entry;

    size_t maxlen = 0;
    for(entry = &commands[0]; entry->name; entry++) {
        size_t len = strlen(entry->name);
        if(maxlen < len) {
            maxlen = len;
        }
    }

    console_write_string("Available commands:" CONSOLE_EOL);
    for(entry = &commands[0]; entry->name; entry++) {
        console_write_string(entry->name);
        console_write_string(": ");
        for(size_t i = 0; i < maxlen - strlen(entry->name); i++) {
            console_write_string(" ");
        }
        console_write_string(entry->help);
        console_write_string(CONSOLE_EOL);
    }
}

const struct console_command_entry commands[] = {
    { "read-io", cmd_read_io, "Read GPIO" },
    { "write-io", cmd_write_io, "Write GPIO" },
    { "write-iodir", cmd_write_iodir, "Write IO directions" },
    { "write-pullup", cmd_write_pullup, "Write IO pullups" },
    { "read-iodir", cmd_read_iodir, "Read IO directions" },
    { "read-pullup", cmd_read_pullup, "Read IO pullups" },
    { "set-anim", cmd_set_anim, "Set animation 0-3" },
    { "echo", cmd_echo, "Echo on/off" },
    { "test", cmd_test, "A simple test" },
    { "test-u16", cmd_test_u16, "A test with a parameter" },
    { "version", cmd_version, "Get version of firmware" },
    { "help", cmd_help, "Display this message" },
    { "usage", cmd_help, "Display this message" },
    { 0, 0, 0 },
};
