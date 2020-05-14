#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tokenizer.h"

#define CHAR_NONE -2


static const char literals[] = "&|^~(),=+-*/%;";

struct keyword_pair {
    const char* name;
    enum token token;
};

const struct keyword_pair keywords[] = {
    { "end", TOKEN_END },
    { "loop", TOKEN_LOOP },
    { "repeat", TOKEN_REPEAT },
    { "let", TOKEN_LET },
    { "bits", TOKEN_BITS },
    { NULL, TOKEN_NONE },
};


void tokenizer_init(struct tokenizer_context *ctx, FILE *fp)
{
    ctx->unread_char = CHAR_NONE;
    ctx->token = TOKEN_NONE;
    ctx->line = 0;
    ctx->fp = fp;
}

void tokenizer_unread_char(struct tokenizer_context *ctx, int c)
{
    ctx->unread_char = c;
}

int tokenizer_read_char_raw(struct tokenizer_context *ctx)
{
    if(ctx->unread_char != CHAR_NONE) {
        int c = ctx->unread_char;
        ctx->unread_char = CHAR_NONE;
        return c;
    }

    int c = fgetc(ctx->fp);
    if(c == '\n') {
        ctx->line++;
    }
    return c;
}

int tokenizer_read_char(struct tokenizer_context *ctx)
{
    int c = tokenizer_read_char_raw(ctx);
    if(c == '#') {
        do {
            c = tokenizer_read_char_raw(ctx);
        } while(!((c == '\n') || (c < 0)));
    }
    return c;
}

static int next_char_is(struct tokenizer_context *ctx, int expect)
{
    int c = tokenizer_read_char(ctx);
    if(c == expect) {
        return 1;
    }
    tokenizer_unread_char(ctx, c);
    return 0;
}


enum token token_peek(struct tokenizer_context *ctx)
{
    if(ctx->token) {
        return ctx->token;
    }

    int c;
    do {
        c = tokenizer_read_char(ctx);
    } while(isblank(c));

    if(c == -1) {
        ctx->token = TOKEN_EOF;
    } else if(strchr(literals, c)) {
        ctx->token =  c;
    } else if((c == '\n') || (c == '\r')) {
        ctx->token =  TOKEN_EOL;
    } else if(c == '<') {
        if(next_char_is(ctx, '<')) {
            ctx->token =  TOKEN_SHIFTLEFT;
        } else if(next_char_is(ctx, '=')) {
            ctx->token =  TOKEN_SMALLEREQUAL;
        } else {
            ctx->token =  '<';
        }
    } else if(c == '>') {
        if(next_char_is(ctx, '>')) {
            ctx->token =  TOKEN_SHIFTRIGHT;
        } else if(next_char_is(ctx, '=')) {
            ctx->token =  TOKEN_GREATEREQUAL;
        } else {
            ctx->token =  '>';
        }
    } else if(c == '!') {
        if(next_char_is(ctx, '=')) {
            ctx->token = TOKEN_NOTEQUAL;
        } else {
            ctx->token = '!';
        }
    } else if(isdigit(c)) {
        ctx->token = TOKEN_NUMBER;
        int check_radix = c == '0';
        int i = 0;
        ctx->value[i++] = c;

        c = tokenizer_read_char(ctx);

        if(check_radix) {
            if(c == 'x' || c == 'X') { // Radix 16
                do {
                    ctx->value[i++] = c;
                    c = tokenizer_read_char(ctx);
                } while(isxdigit(c) && (i < TOKEN_VALUE_LEN));
                tokenizer_unread_char(ctx, c);
                ctx->value[i] = 0;
            } else if(c == 'b' || c == 'B') { // Radix 2
                do {
                    ctx->value[i++] = c;
                    c = tokenizer_read_char(ctx);
                } while((c == '0' || c == '1') && (i < TOKEN_VALUE_LEN));
                tokenizer_unread_char(ctx, c);
                ctx->value[i] = 0;
            } else { // Radix 8
                while(('0' <= c) && (c < '8') && (i < TOKEN_VALUE_LEN)) {
                    ctx->value[i++] = c;
                    c = tokenizer_read_char(ctx);
                }
                tokenizer_unread_char(ctx, c);
                ctx->value[i] = 0;
            }
        } else { // Radix 10
            while(isdigit(c) && (i < TOKEN_VALUE_LEN)) {
                ctx->value[i++] = c;
                c = tokenizer_read_char(ctx);
            }
            tokenizer_unread_char(ctx, c);
            ctx->value[i] = 0;
        }
    } else if(isalpha(c) || c == '_') {
        ctx->token = TOKEN_IDENT;

        int i = 0;

        do {
            ctx->value[i++] = c;
            c = tokenizer_read_char(ctx);
        } while((isalnum(c) || c == '_') && (i < TOKEN_VALUE_LEN));

        tokenizer_unread_char(ctx, c);
        ctx->value[i] = 0;

        for(const struct keyword_pair *kw = keywords; kw->name; kw++) {
            if(strcmp(ctx->value, kw->name) == 0) {
                ctx->token = kw->token;
                break;
            }
        }

    } else {
        ctx->token =  TOKEN_UNKNOWN;
    }

    return ctx->token;
}

void token_consume(struct tokenizer_context *ctx)
{
    ctx->token = TOKEN_NONE;
}

number_t token_get_number(struct tokenizer_context *ctx)
{
    if((ctx->value[0] == '0') && (strlen(ctx->value) > 1)) {
        if((ctx->value[1] == 'x') || (ctx->value[1] == 'X')) {
            return strtoll(&ctx->value[2], NULL, 16);
        }
        if((ctx->value[1] == 'b') || (ctx->value[1] == 'B')) {
            return strtoll(&ctx->value[2], NULL, 2);
        }

        return strtoll(&ctx->value[1], NULL, 8);
    }

    return strtoll(ctx->value, NULL, 10);
}

int token_match(enum token type, const enum token *list)
{
    while(*list) {
        if(type == *list++) return 1;
    }
    return 0;
}

int token_accept(struct tokenizer_context *ctx, enum token tok)
{
    if(token_peek(ctx) == tok) {
        token_consume(ctx);
        return 1;
    }
    return 0;
}

// LCOV_EXCL_START
const char* token_to_string(enum token token)
{
    switch(token) {
    case TOKEN_NONE: return "NONE";
    case TOKEN_UNKNOWN: return "UNKNOWN";
    case TOKEN_IDENT: return "IDENT";
    case TOKEN_NUMBER: return "NUMBER";
    case TOKEN_EOL: return "EOL";
    case TOKEN_EOF: return "EOF";
    case TOKEN_SHIFTLEFT: return "SHIFTLEFT";
    case TOKEN_SHIFTRIGHT: return "SHIFTRIGHT";
    case TOKEN_GREATEREQUAL: return "GREATEREQUAL";
    case TOKEN_SMALLEREQUAL: return "SMALLEREQUAL";
    case TOKEN_END: return "END";
    case TOKEN_LOOP: return "LOOP";
    case TOKEN_REPEAT: return "REPEAT";
    case TOKEN_BITS: return "BITS";
    case TOKEN_LET: return "LET";
    case TOKEN_NOTEQUAL: return "NOTEQUAL";
    case TOKEN_AND: return "AND";
    case TOKEN_OR: return "OR";
    case TOKEN_XOR: return "XOR";
    case TOKEN_BIN_NOT: return "BIN_NOT";
    case TOKEN_OPEN: return "OPEN";
    case TOKEN_CLOSE: return "CLOSE";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_EQUAL: return "EQUAL";
    case TOKEN_ADD: return "ADD";
    case TOKEN_SUB: return "SUB";
    case TOKEN_MUL: return "MUL";
    case TOKEN_GREATER: return "GREATER";
    case TOKEN_SMALLER: return "SMALLER";
    case TOKEN_DIV: return "DIV";
    case TOKEN_MOD: return "MOD";
    case TOKEN_SEMICOLON: return "SEMICOLON";
    case TOKEN_LOG_NOT: return "LOG_NOT";
    }
    return "Not a token";
}
// LCOV_EXCL_STOP
