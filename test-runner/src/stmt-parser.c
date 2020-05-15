#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "util.h"
#include "tokenizer.h"
#include "parser.h"


#ifndef UNIT_TESTING
static struct stmt* new_stmt(enum stmt_type type)
{
    struct stmt *stmt = allocate(sizeof(*stmt));
    stmt->type = type;
    return stmt;
}
#else
static struct stmt* test_new_stmt(enum stmt_type type,const char* file, const int line)
{
    struct stmt *stmt = test_allocate(sizeof(*stmt),file,line);
    stmt->type = type;
    return stmt;
}
#define new_stmt(type) test_new_stmt(type, __FILE__, __LINE__)
#endif

void free_stmt(struct stmt *stmt)
{
    while(stmt) {
        if(stmt->type == STMT_LOOP) {
            free(stmt->name);
            free_stmt(stmt->stmt);
        } else if(stmt->type == STMT_REPEAT) {
            free_data(stmt->data);
        } else if(stmt->type == STMT_LET) {
            free(stmt->name);
            free_expr(stmt->expr);
        } else if(stmt->type == STMT_DATA_ROW) {
            free_data(stmt->data);
        }

        struct stmt *next = stmt->next;
        free(stmt);
        stmt = next;
    }
}

static struct data *parse_data_row(struct tokenizer_context *ctx)
{
    const enum token row_tokens[] = { TOKEN_NUMBER, TOKEN_IDENT, TOKEN_BITS, '(', 0 };

    struct data *first = 0;
    struct data *prev = 0;

    while(token_match(token_peek(ctx), row_tokens)) {
        struct data *data = parse_data(ctx);
        if(!data) {
            goto err;
        }

        if(prev) {
            prev->next= data;
        } else {
            first = data;
        }

        prev = data;
    }

    return first;
err:
    free_data(first);
    return 0;
}

struct stmt *parse_stmt(struct tokenizer_context *ctx)
{
    while(token_accept(ctx, '\n')) {
    }

    enum token tok = token_peek(ctx);
    struct stmt *stmt = 0;

    const enum token row_tokens[] = { TOKEN_NUMBER, TOKEN_IDENT, TOKEN_BITS, '(', 0 };

    if(token_match(tok, row_tokens)) {
        stmt = new_stmt(STMT_DATA_ROW);
        stmt->data = parse_data_row(ctx);
        if(!stmt->data) {
            goto err;
        }
        if(!(token_accept(ctx,'\n') || token_accept(ctx,TOKEN_EOF))) {
            parse_error(ctx, "Expected end of line");
        }
    } else if(tok == TOKEN_LET) {
        stmt = new_stmt(tok);

        token_consume(ctx);

        if(token_peek(ctx) != TOKEN_IDENT) {
            parse_error(ctx, "Expected variable name");
        }

        stmt->name = allocate(strlen(ctx->value)+1);
        strcpy(stmt->name, ctx->value);
        token_consume(ctx);

        token_expect(ctx, '=');

        stmt->expr = parse_expr(ctx);

        if(!stmt->expr) {
            goto err;
        }

        token_expect(ctx, ';');
    } else if(tok == TOKEN_LOOP) {
        stmt = new_stmt(tok);

        token_consume(ctx);
        token_expect(ctx, '(');

        if(token_peek(ctx) != TOKEN_IDENT) {
            parse_error(ctx, "Expected variable name");
        }

        stmt->name = allocate(strlen(ctx->value)+1);
        strcpy(stmt->name, ctx->value);
        token_consume(ctx);

        token_expect(ctx, ',');

        if(token_peek(ctx) != TOKEN_NUMBER) {
            parse_error(ctx, "Expected a number");
        }

        stmt->number = token_get_number(ctx);
        token_consume(ctx);

        token_expect(ctx, ')');

        stmt->stmt = parse_stmt(ctx);

        if(!stmt->stmt) {
            goto err;
        }

        token_expect(ctx, TOKEN_END);
        token_expect(ctx, TOKEN_LOOP);
    } else if(tok == TOKEN_REPEAT) {
        stmt = new_stmt(tok);
        token_consume(ctx);

        token_expect(ctx, '(');

        if(token_peek(ctx) != TOKEN_NUMBER) {
            parse_error(ctx, "Expected a number");
        }

        stmt->number = token_get_number(ctx);
        token_consume(ctx);

        token_expect(ctx, ')');

        stmt->data = parse_data_row(ctx);
        if(!stmt->data) {
            goto err;
        }
        if(!(token_accept(ctx,'\n') || token_accept(ctx,TOKEN_EOF))) {
            parse_error(ctx, "Expected end of line");
        }
    } else if(tok != TOKEN_EOF) {
        print_error(ctx, "Unexpected token of type %s", token_to_string(ctx->token));
    }

    return stmt;

err:
    free_stmt(stmt);
    return 0;
}
