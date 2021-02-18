#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"

#define PIN_CHIP_MAX 4

enum expr_type {
    EXPR_SHIFTLEFT = TOKEN_SHIFTLEFT,
    EXPR_SHIFTRIGHT = TOKEN_SHIFTRIGHT,
    EXPR_GREATEREQUAL = TOKEN_GREATEREQUAL,
    EXPR_SMALLEREQUAL = TOKEN_SMALLEREQUAL,
    EXPR_NOTEQUAL = TOKEN_NOTEQUAL,
    EXPR_GREATER = TOKEN_GREATER,
    EXPR_SMALLER = TOKEN_SMALLER,

    EXPR_AND = TOKEN_AND,
    EXPR_OR = TOKEN_OR,
    EXPR_XOR = TOKEN_XOR,
    EXPR_EQUAL = TOKEN_EQUAL,
    EXPR_ADD = TOKEN_ADD,
    EXPR_SUB = TOKEN_SUB,
    EXPR_MUL = TOKEN_MUL,
    EXPR_DIV = TOKEN_DIV,
    EXPR_MOD = TOKEN_MOD,

    EXPR_BIN_NOT = TOKEN_BIN_NOT,
    EXPR_LOG_NOT = TOKEN_LOG_NOT,
    EXPR_UNARY_MINUS = 'm',

    EXPR_NUMBER = TOKEN_NUMBER,
    EXPR_FUNCTION = 'f',
    EXPR_VARIABLE = 'v',
};

enum stmt_type {
    STMT_LOOP = TOKEN_LOOP,
    STMT_REPEAT = TOKEN_REPEAT,
    STMT_LET = TOKEN_LET,
    STMT_DATA_ROW = 'd',
};

enum data_type {
    DATA_NUMBER = TOKEN_NUMBER,
    DATA_BITS = TOKEN_BITS,
    DATA_IDENT = TOKEN_IDENT,
    DATA_EXPR = 'E',
};

typedef number_t (*function_ptr)(number_t param[]);

struct function_param {
    struct expr *expr;
    struct function_param *next;
};

struct function {
    const char *name;
    int nparams;
    function_ptr func;
};

struct expr {
    enum expr_type type;

    union {
        struct { // binary
            struct expr *left;
            struct expr *right;
        };
        struct { // unary
            struct expr *child;
        };
        struct { // number
            number_t number;
        };
        struct { // variable
            char *name;
        };
        struct { // function
            struct function* function;
            struct function_param *param;
        };
    };
};

struct data {
    enum data_type type;
    struct data *next;

    union {
        struct { // expression / bits
            struct expr *expr;
            number_t number;
        };
        struct { // identifier
            char ident;
        };
    };
};

struct stmt {
    enum stmt_type type;
    struct stmt *next;

    union {
        char *name; // let, loop
        struct data *data; // data, repeat
    };
    union {
        struct expr *expr; // let
        struct stmt *stmt; // loop
    };
    number_t number; // loop, data
    int line;
};

enum signal_type {
    SIGNAL_NONE,
    SIGNAL_INPUT,
    SIGNAL_OUTPUT,
};

struct pin {
    struct pin *next;
    int number;
    int bank;
};

struct signal {
    char *name;
    struct signal *next;
    enum signal_type type;
    struct pin *pin;
};

struct test_file {
    struct stmt *stmt;
    struct signal *signal;
};

struct variable {
    char *name;
    number_t value;
    struct variable *next;
};

struct eval_context {
    struct eval_context *up;
    struct variable *variable;
    const char *error;
};

struct expr *parse_expr(struct tokenizer_context *ctx);
struct data *parse_data(struct tokenizer_context *ctx);
struct stmt *parse_stmt(struct tokenizer_context *ctx);
struct stmt *parse_stmts(struct tokenizer_context *ctx);
struct signal *parse_header(struct tokenizer_context *ctx);

struct test_file *test_file_open(const char *filename);

#define test_file_close free_test_file

void free_expr(struct expr*);
void free_stmt(struct stmt*);
void free_data(struct data*);
void free_signal(struct signal *signal);
void free_test_file(struct test_file *test_file);
void free_eval_context(struct eval_context *eval_ctx);

number_t eval_expr(struct expr *expr, struct eval_context *eval_ctx);

void set_variable(struct eval_context *eval_ctx, const char *name, number_t value);
number_t get_variable(struct eval_context *eval_ctx, const char *name);

struct data *eval_data_row(struct data *data_in, struct eval_context *eval_ctx);
struct stmt *eval_stmts(struct stmt *stmt_in, struct eval_context *eval_ctx);

extern struct function function_tab[];

#endif
