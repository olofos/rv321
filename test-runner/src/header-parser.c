#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tokenizer.h"
#include "parser.h"
#include "util.h"

static void free_pin(struct pin *pin)
{
    while(pin) {
        struct pin *next = pin->next;
        free(pin);
        pin = next;
    }
}

void free_signal(struct signal *signal)
{
    while(signal) {
        struct signal *next = signal->next;
        free(signal->name);
        free_pin(signal->pin);
        free(signal);
        signal = next;
    }
}

static void skip_empty_lines_and_comments(struct tokenizer_context *ctx)
{
    int c;
    for(;;) {
        c = tokenizer_read_char(ctx);
        if(!isspace(c)) {
            tokenizer_unread_char(ctx,c);
            return;
        }
    }
}

static char *read_identifier(struct tokenizer_context *ctx)
{
    int c;
    size_t buf_size = 0;
    size_t index = 0;
    char *ident = 0;

    do {
        c = tokenizer_read_char_raw(ctx);
    } while(isblank(c));
    tokenizer_unread_char(ctx, c);

    for(;;) {
        c = tokenizer_read_char_raw(ctx);

        if(c == -1) {
            print_error(ctx, "Unexpected EOF");
            goto err;
        }
        if(isspace(c)) {
            tokenizer_unread_char(ctx, c);

            if(ident) {
                ident[index] = 0;
                ident = reallocate(ident, strlen(ident)+1);
            }

            break;
        }
        if(index + 1 >= buf_size) {
            if(buf_size == 0) {
                buf_size = TOKEN_VALUE_LEN + 1;
            } else {
                buf_size = 2 * (buf_size - 1) + 1;
            }
            ident = reallocate(ident, buf_size);
        }
        ident[index++] = c;
    }

    return ident;
err:
    free(ident);
    return 0;
}

static struct signal *read_signals(struct tokenizer_context *ctx)
{
    struct signal *first = 0, *prev = 0;
    char *ident;

    while((ident = read_identifier(ctx))) {
        struct signal *signal = allocate(sizeof(*signal));
        signal->name = ident;

        if(prev) {
            prev->next = signal;
        } else {
            first = signal;
        }
        prev = signal;
    }

    if(!first) {
        goto err;
    }

    int c = tokenizer_read_char(ctx);

    if(c != '\n') {
        print_error(ctx, "Expected a newline");
        goto err;
    }

    return first;

err:
    free_signal(first);
    return 0;
}

static struct signal *find_signal(struct signal *first, const char *name)
{
    for(struct signal *signal = first; signal; signal = signal->next) {
        if(strcmp(signal->name, name) == 0) {
            return signal;
        }
    }
    return 0;
}


static int check_for_duplicate_signals(struct signal *first)
{
    int dup_found = 0;
    for(struct signal *signal = first; signal; signal = signal->next) {
        if(find_signal(signal->next, signal->name)) {
            print_error(0, "Error: Signal '%s' appears twice", signal->name);
            dup_found = 1;
        }
    }

    return dup_found;
}

static struct pin *read_pins(struct tokenizer_context *ctx)
{
    struct pin *first = 0, *prev = 0;

    char *ident;

    while((ident = read_identifier(ctx))) {
        if((ident[0] == 'X') || (ident[0] == 'x')) {
            int skip_num = 1;
            if(ident[1] == '[') {
                char *endp;
                skip_num = strtol(&ident[2], &endp, 10);
                if((skip_num <= 0) || (strcmp(endp, "]") != 0)) {
                    print_error(ctx, "Unexpected pin %s", ident);
                    free(ident);
                    goto err;
                }
            } else if(ident[1] != 0) {
                print_error(ctx, "Unexpected pin %s", ident);
                free(ident);
                goto err;
            }

            for(int i = 0; i < skip_num; i++) {
                struct pin *pin = allocate(sizeof(*pin));
                pin->number = PIN_SKIP;
                pin->bank = PIN_SKIP;

                if(prev) {
                    prev->next = pin;
                } else {
                    first = pin;
                }
                prev = pin;
            }
        } else {
            if(strlen(ident) != 3) {
                print_error(ctx, "Unexpected pin %s", ident);
                free(ident);
                goto err;
            }

            if((ident[0] < '0') || (ident[0] > '0' + PIN_CHIP_MAX)) {
                print_error(ctx, "Unexpected chip number %c", ident[0]);
                free(ident);
                goto err;
            }

            if((ident[1] != 'A') && (ident[1] != 'a') && (ident[1] != 'B') && (ident[1] != 'b')) {
                print_error(ctx, "Unexpected bank %c", ident[1]);
                free(ident);
                goto err;
            }

            if((ident[2] < '0') || (ident[2] > '7')) {
                print_error(ctx, "Unexpected pin number %c", ident[2]);
                free(ident);
                goto err;
            }

            int chip = ident[0] - '0';
            int bank = ((ident[1] == 'B') || (ident[1] == 'b')) ? 1 : 0;
            int number = ident[2] - '0';

            struct pin *pin = allocate(sizeof(*pin));
            pin->number = number;
            pin->bank = (chip << 1) | bank;

            if(prev) {
                prev->next = pin;
            } else {
                first = pin;
            }
            prev = pin;
        }

        free(ident);
    }

    return first;

err:
    free_pin(first);
    return 0;
}

static int is_signal_type(const char *str)
{
    const char *ok[] = { "input", "in", "output", "out", "ignore" };

    for(int i = 0; i < ARRAYSIZE(ok); i++) {
        if(strcasecmp(str, ok[i]) == 0) return 1;
    }

    return 0;
}

static enum signal_type to_signal_type(const char *str)
{
    if(strncasecmp("in", str, 2) == 0) return SIGNAL_INPUT;
    if(strncasecmp("out", str, 3) == 0) return SIGNAL_OUTPUT;
    return SIGNAL_NONE;
}

static int read_mapping(struct tokenizer_context *ctx, struct signal *first_signal)
{
    enum token tok = token_peek(ctx);
    if(tok != TOKEN_COMMAND) {
        return 1;
    }
    token_consume(ctx);

    char *name = 0;
    struct pin *pin = 0;

    name = read_identifier(ctx);

    if((token_peek(ctx) != TOKEN_IDENT) || !is_signal_type(ctx->value)) {
        parse_error(ctx, "Expected INPUT, OUTPUT or IGNORE", token_to_string(ctx->token));
    }
    enum signal_type type = to_signal_type(ctx->value);
    token_consume(ctx);

    pin = read_pins(ctx);

    if(type == SIGNAL_NONE) {
        if(pin) {
            parse_error(ctx, "Unexpected pin assignmet for ignored signal %s", name);
        }
    } else {
        if(!pin) {
            parse_error(ctx, "Missing pin assignment for signal %s", name);
        }
    }

    struct signal *signal = find_signal(first_signal, name);

    if(!signal) {
        parse_error(ctx, "Unknown signal %s", name);
    }

    if(signal->pin) {
        parse_error(ctx, "Signal %s is already mapped", signal->name);
    }

    signal->type = type;
    signal->pin = pin;

    free(name);
    return 1;

err:
    free_pin(pin);
    free(name);
    return 0;
}

static int read_mappings(struct tokenizer_context *ctx, struct signal *first_signal)
{
    do {
        if(!read_mapping(ctx, first_signal)) {
            return 0;
        }
        if(token_peek(ctx) == TOKEN_EOF) {
            return 0;
        }
    } while(token_accept(ctx, '\n'));

    return 1;
}


struct signal *parse_header(struct tokenizer_context *ctx)
{
    skip_empty_lines_and_comments(ctx);
    struct signal *signal;

    if(!(signal = read_signals(ctx))) {
        goto err;
    }

    if(check_for_duplicate_signals(signal)) {
        goto err;
    }

    if(!read_mappings(ctx, signal)) {
        goto err;
    }

    return signal;

err:
    free_signal(signal);
    return 0;
}
