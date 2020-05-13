#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#define TOKEN_VALUE_LEN 32

typedef int64_t number_t;

enum token {
    TOKEN_NONE = 0,
    TOKEN_UNKNOWN = 'X',
    TOKEN_IDENT = 'I',
    TOKEN_NUMBER = '#',
    TOKEN_EOL = '\n',
    TOKEN_EOF = 'q',
    TOKEN_SHIFTLEFT = 'L',
    TOKEN_SHIFTRIGHT = 'R',
    TOKEN_GREATEREQUAL = 'G',
    TOKEN_SMALLEREQUAL = 'S',
    TOKEN_END = 'e',
    TOKEN_LOOP = 'l',
    TOKEN_REPEAT = 'r',
    TOKEN_LET = '@',
    TOKEN_NOTEQUAL = '?',
    TOKEN_BITS = 'b',

    TOKEN_AND = '&',
    TOKEN_OR = '|',
    TOKEN_XOR = '^',
    TOKEN_BIN_NOT = '~',
    TOKEN_OPEN = '(',
    TOKEN_CLOSE = ')',
    TOKEN_COMMA = ',',
    TOKEN_EQUAL = '=',
    TOKEN_ADD = '+',
    TOKEN_SUB = '-',
    TOKEN_MUL = '*',
    TOKEN_GREATER = '>',
    TOKEN_SMALLER = '<',
    TOKEN_DIV = '/',
    TOKEN_MOD = '%',
    TOKEN_SEMICOLON = ';',
    TOKEN_LOG_NOT = '!',
};

struct tokenizer_context {
    int unread_char;
    int token;
    int line;
    FILE *fp;
    char value[TOKEN_VALUE_LEN+1];
};

void tokenizer_init(struct tokenizer_context *ctx, FILE *fp);
enum token token_peek(struct tokenizer_context *ctx);
void token_consume(struct tokenizer_context *ctx);
number_t token_get_number(struct tokenizer_context *ctx);
int token_match(enum token type, const enum token *list);
const char* token_to_string(enum token token);
int token_accept(struct tokenizer_context *ctx, enum token tok);

#define token_expect(ctx, tok)                                          \
    do {                                                                \
        if(token_peek(ctx) != tok) {                                    \
            parse_error(ctx, "Expected %s but found %s", token_to_string(tok), token_to_string(ctx->token)); \
        }                                                               \
        token_consume(ctx);                                             \
    } while(0)


#endif
