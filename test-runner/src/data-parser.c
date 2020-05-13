#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "util.h"
#include "tokenizer.h"
#include "parser.h"


#ifndef UNIT_TESTING
static struct data* new_data(enum data_type type)
{
    struct data *data = allocate(sizeof(*data));
    data->type = type;
    return data;
}
#else
static struct data* test_new_data(enum data_type type,const char* file, const int line)
{
    struct data *data = test_allocate(sizeof(*data),file,line);
    data->type = type;
    return data;
}
#define new_data(type) test_new_data(type, __FILE__, __LINE__)
#endif

void free_data(struct data *data)
{
    while(data) {
        if((data->type == DATA_EXPR) || (data->type == DATA_BITS)) {
            free_expr(data->expr);
        }
        struct data *next = data->next;
        free(data);
        data = next;
    }
}

struct data *parse_data(struct tokenizer_context *ctx)
{
    enum token tok = token_peek(ctx);
    struct data *data = 0;

    if(tok == TOKEN_NUMBER) {
        data = new_data(tok);
        data->number = token_get_number(ctx);
        token_consume(ctx);
    } else if(tok == TOKEN_BITS) {
        data = new_data(tok);
        token_consume(ctx);
        token_expect(ctx, '(');
        if(token_peek(ctx) != TOKEN_NUMBER) {
            parse_error(ctx, "Expected a number");
        }
        data->number = token_get_number(ctx);
        token_consume(ctx);
        token_expect(ctx, ',');
        data->expr = parse_expr(ctx);
        if(!data->expr) {
            goto err;
        }
        token_expect(ctx, ')');
    } else if(tok == TOKEN_IDENT) {
        data = new_data(tok);
        if((strlen(ctx->value) == 1) && strchr("ZXCzxc", ctx->value[0])) {
            data->ident = toupper(ctx->value[0]);
            token_consume(ctx);
        } else {
            parse_error(ctx, "Unexpected identifier %s", ctx->value);
        }
    } else if(tok == '(') {
        token_consume(ctx);
        data = new_data(DATA_EXPR);
        data->expr = parse_expr(ctx);
        if(!data->expr) {
            goto err;
        }
        token_expect(ctx, ')');
    }

    return data;
err:
    free_data(data);
    return 0;
}
