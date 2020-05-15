#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "util.h"
#include "tokenizer.h"
#include "parser.h"

static int expr_type_match(enum expr_type type, const enum expr_type *list)
{
    while(*list) {
        if(type == *list++) return 1;
    }
    return 0;
}

static int is_binary(enum expr_type type)
{
    const enum expr_type list[] = { '+', '-', '*', '/', '%', '=', '<', '>','&', '|', '^',
                                    EXPR_GREATEREQUAL, EXPR_SMALLEREQUAL, EXPR_NOTEQUAL,
                                    EXPR_SHIFTLEFT, EXPR_SHIFTRIGHT, 0};
    return expr_type_match(type, list);
}

static int is_unary(enum expr_type type)
{
    const enum expr_type list[] = { EXPR_UNARY_MINUS, '~', '!', 0 };
    return expr_type_match(type, list);
}

#ifndef UNIT_TESTING
static struct expr* new_expr(enum expr_type type)
{
    struct expr *expr = allocate(sizeof(*expr));
    expr->type = type;
    return expr;
}
#else
static struct expr* test_new_expr(enum expr_type type,const char* file, const int line)
{
    struct expr *expr = test_allocate(sizeof(*expr),file,line);
    expr->type = type;
    return expr;
}
#define new_expr(type) test_new_expr(type, __FILE__, __LINE__)

#endif

void free_expr(struct expr *expr)
{
    if(!expr) return;

    if(is_binary(expr->type)) {
        free_expr(expr->left);
        free_expr(expr->right);
    } else if(is_unary(expr->type)) {
        free_expr(expr->child);
    } else if(expr->type == EXPR_VARIABLE) {
        free(expr->name);
    } else if(expr->type == EXPR_FUNCTION) {
        struct function_param *param = expr->param;
        while(param) {
            struct function_param *next = param->next;
            free_expr(param->expr);
            free(param);
            param = next;
        }
    }
    free(expr);
}

static struct expr *parse_function_or_variable(struct tokenizer_context *ctx)
{
    struct expr *expr = 0;

    char name[strlen(ctx->value)+1];
    strcpy(name, ctx->value);
    token_consume(ctx);

    if(token_accept(ctx, '(')) {
        expr = new_expr(EXPR_FUNCTION);

        for(struct function *func = function_tab; func->name; func++) {
            if(strcmp(name, func->name) == 0) {
                expr->function = func;
                break;
            }
        }

        if(!expr->function) {
            parse_error(ctx, "Unknown function %s", name);
        }

        int nparams = 0;
        struct function_param *param = 0;

        do {
            if(token_peek(ctx) == ')') {
                break;
            }

            struct expr *pexpr = parse_expr(ctx);
            if(!pexpr){
                break;
            }

            struct function_param *p = allocate(sizeof(*p));
            p->expr = pexpr;

            if(param) {
                param->next = p;
            } else {
                expr->param = p;
            }

            nparams++;
            param = p;
        } while(token_accept(ctx, ','));

        token_expect(ctx, ')');

        if(nparams != expr->function->nparams) {
            parse_error(ctx, "Function %s expected %d parameters but %d were found\n", expr->function->name, expr->function->nparams, nparams);
            goto err;
        }
    } else {
        expr = new_expr(EXPR_VARIABLE);
        expr->name = allocate(strlen(name)+1);
        strcpy(expr->name, name);
    }

    return expr;

err:
    free_expr(expr);
    return 0;
}

static struct expr *parse_factor(struct tokenizer_context *ctx)
{
    struct expr *expr = 0;

    enum token tok = token_peek(ctx);

    if(tok == TOKEN_NUMBER) {
        expr = new_expr(tok);
        expr->number = token_get_number(ctx);
        token_consume(ctx);
    } else if(tok == '-') {
        expr = new_expr(EXPR_UNARY_MINUS);
        token_consume(ctx);
        expr->child = parse_factor(ctx);
    } else if((tok == '~') || (tok == '!')) {
        expr = new_expr(tok);
        token_consume(ctx);
        expr->child = parse_factor(ctx);
    } else if(tok == '(') {
        token_consume(ctx);
        expr = parse_expr(ctx);
        token_expect(ctx, ')');
    } else if(tok == TOKEN_IDENT) {
        expr = parse_function_or_variable(ctx);
    } else {
        print_error(ctx, "Unexpected token of type %s", token_to_string(ctx->token));
    }

    return expr;

err:
    free_expr(expr);
    return 0;
}

typedef struct expr* parse_func_t(struct tokenizer_context *ctx);

static struct expr* parse_binary_helper(struct tokenizer_context *ctx, parse_func_t child_func, enum token *token_list)
{
    struct expr* expr = child_func(ctx);
    if(expr) {
        enum token tok = token_peek(ctx);
        while(token_match(tok, token_list)) {
            enum token op = tok;
            token_consume(ctx);
            struct expr* left = expr;
            struct expr* right = child_func(ctx);
            struct expr *n = new_expr(op);
            n->left = left;
            n->right = right;
            tok = token_peek(ctx);
            expr = n;
            if(!left || !right) goto err;
        }
    }
    return expr;
err:
    free_expr(expr);
    return 0;
}

static struct expr* parse_term(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_factor, (enum token[]) {'*', '/', '%', 0});
}

static struct expr* parse_arithm_expr(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_term, (enum token[]) {'+', '-', 0});
}

static struct expr* parse_shift_expr(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_arithm_expr, (enum token[]) {TOKEN_SHIFTLEFT, TOKEN_SHIFTRIGHT, 0});
}

static struct expr* parse_and(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_shift_expr, (enum token[]) {'&', 0});
}

static struct expr* parse_xor(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_and, (enum token[]) {'^', 0});
}

static struct expr* parse_or(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_xor, (enum token[]) {'|', 0});
}

struct expr* parse_expr(struct tokenizer_context *ctx)
{
    return parse_binary_helper(ctx, parse_or, (enum token[]) {'<', '=', '>', TOKEN_GREATEREQUAL, TOKEN_SMALLEREQUAL, TOKEN_NOTEQUAL, 0});
}
