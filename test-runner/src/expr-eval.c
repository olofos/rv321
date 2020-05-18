#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "util.h"

void free_eval_context(struct eval_context *eval_ctx)
{
    if(!eval_ctx) return;

    struct variable *variable = eval_ctx->variable;

    while(variable) {
        struct variable *next = variable->next;
        free(variable->name);
        free(variable);
        variable = next;
    }

    free(eval_ctx);
}

void set_variable(struct eval_context *eval_ctx, const char *name, number_t value)
{
    for(struct variable *variable = eval_ctx->variable; variable; variable = variable->next) {
        if(strcmp(variable->name, name) == 0) {
            variable->value = value;
            return;
        }
    }

    struct variable *variable = allocate(sizeof(*variable));
    variable->name = allocate(strlen(name)+1);
    strcpy(variable->name, name);
    variable->value = value;
    variable->next = eval_ctx->variable;
    eval_ctx->variable = variable;
}

number_t get_variable(struct eval_context *first_eval_ctx, const char *name)
{
    for(struct eval_context *eval_ctx = first_eval_ctx; eval_ctx; eval_ctx = eval_ctx->up) {
        for(struct variable *variable = eval_ctx->variable; variable; variable = variable->next) {
            if(strcmp(variable->name, name) == 0) {
                return variable->value;
            }
        }
    }

    first_eval_ctx->error = "Variable not found";
    return 0;
}

number_t eval_expr(struct expr *expr, struct eval_context *eval_ctx)
{
    if(eval_ctx->error) {
        return 0;
    }

    switch(expr->type) {
    case EXPR_NUMBER: return expr->number;
    case '+': return eval_expr(expr->left, eval_ctx) + eval_expr(expr->right, eval_ctx);
    case '-': return eval_expr(expr->left, eval_ctx) - eval_expr(expr->right, eval_ctx);
    case '*': return eval_expr(expr->left, eval_ctx) * eval_expr(expr->right, eval_ctx);
    case '/':
    {
        number_t left = eval_expr(expr->left, eval_ctx);
        number_t right = eval_expr(expr->right, eval_ctx);

        if(eval_ctx->error) return 0;
        if(right == 0) {
            eval_ctx->error = "Division by 0";
            return 0;
        }
        return  left / right;
    }
    case '%':
    {
        number_t left = eval_expr(expr->left, eval_ctx);
        number_t right = eval_expr(expr->right, eval_ctx);

        if(eval_ctx->error) return 0;
        if(right == 0) {
            eval_ctx->error = "Division by 0";
            return 0;
        }
        return  left % right;
    }

    case EXPR_SHIFTLEFT: return eval_expr(expr->left, eval_ctx) << eval_expr(expr->right, eval_ctx);
    case EXPR_SHIFTRIGHT: return eval_expr(expr->left, eval_ctx) >> eval_expr(expr->right, eval_ctx);

    case '&': return eval_expr(expr->left, eval_ctx) & eval_expr(expr->right, eval_ctx);
    case '|': return eval_expr(expr->left, eval_ctx) | eval_expr(expr->right, eval_ctx);
    case '^': return eval_expr(expr->left, eval_ctx) ^ eval_expr(expr->right, eval_ctx);

    case '=': return eval_expr(expr->left, eval_ctx) == eval_expr(expr->right, eval_ctx);
    case EXPR_NOTEQUAL: return eval_expr(expr->left, eval_ctx) != eval_expr(expr->right, eval_ctx);
    case '<': return eval_expr(expr->left, eval_ctx) < eval_expr(expr->right, eval_ctx);
    case '>': return eval_expr(expr->left, eval_ctx) > eval_expr(expr->right, eval_ctx);
    case EXPR_SMALLEREQUAL: return eval_expr(expr->left, eval_ctx) <= eval_expr(expr->right, eval_ctx);
    case EXPR_GREATEREQUAL: return eval_expr(expr->left, eval_ctx) >= eval_expr(expr->right, eval_ctx);

    case '!': return !eval_expr(expr->child, eval_ctx);
    case '~': return ~eval_expr(expr->child, eval_ctx);
    case EXPR_UNARY_MINUS: return -eval_expr(expr->child, eval_ctx);

    case EXPR_FUNCTION:
    {
        int nparam = 0;

        for(struct function_param *param = expr->param; param; param = param->next) {
            nparam++;
        }

        number_t params[nparam];

        int n = 0;

        for(struct function_param *param = expr->param; param; param = param->next) {
            params[n++] = eval_expr(param->expr, eval_ctx);
        }

        return expr->function->func(params);
    }

    case EXPR_VARIABLE:
        return get_variable(eval_ctx, expr->name);
    }

    return 0; // LCOV_EXCL_LINE
}
