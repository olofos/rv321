#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "tokenizer.h"
#include "parser.h"
#include "util.h"
#include "mt64.h"

number_t func_random(number_t param[]);
number_t func_ite(number_t param[]);

struct function function_tab[] = {
    { .name = "random", .nparams = 1, .func = func_random },
    { .name = "ite", .nparams = 3, .func = func_ite },
    { .name = 0, .nparams = 0, .func = 0 },
};

number_t func_random(number_t param[])
{
    number_t max = param[0];
    const uint64_t rand_max = 0xFFFFFFFFFFFFFFFF;

    return genrand64_int64() / ( 1 + (rand_max - max + 1) / max );
}

number_t func_ite(number_t param[])
{
    return param[0] ? param[1] : param[2];
}

static int is_simple(struct expr *expr)
{
    enum expr_type list[] = {EXPR_NUMBER, EXPR_VARIABLE, EXPR_FUNCTION};
    for(int i = 0; i < ARRAYSIZE(list); i++) {
        if(expr->type == list[i]) return 1;
    }
    return 0;
}

static void print_expr(struct expr *expr);

static void print_binary(struct expr *expr, const char* s)
{
    if(is_simple(expr->left)) {
        print_expr(expr->left);
    } else {
        printf("( ");
        print_expr(expr->left);
        printf(" )");
    }

    printf(" %s ", s);

    if(is_simple(expr->right)) {
        print_expr(expr->right);
    } else {
        printf("( ");
        print_expr(expr->right);
        printf(" )");
    }
}

static void print_unary(struct expr *expr, const char *s)
{
    printf("%s", s);
    if(is_simple(expr)) {
        print_expr(expr->child);
    } else {
        printf("( ");
        print_expr(expr->child);
        printf(" )");
    }
}

static void print_expr(struct expr *expr)
{
    switch(expr->type) {
    case EXPR_NUMBER:
        printf("%ld", expr->number);
        break;

    case EXPR_VARIABLE:
        printf("%s", expr->name);
        break;

    case EXPR_AND:
    case EXPR_OR:
    case EXPR_XOR:
    case EXPR_EQUAL:
    case EXPR_ADD:
    case EXPR_SUB:
    case EXPR_MUL:
    case EXPR_GREATER:
    case EXPR_SMALLER:
    case EXPR_DIV:
    case EXPR_MOD:
    {
        char s[] = {expr->type, 0};
        print_binary(expr, s);
        break;
    }

    case EXPR_SHIFTLEFT:
        print_binary(expr, "<<");
        break;

    case EXPR_SHIFTRIGHT:
        print_binary(expr, ">>");
        break;

    case EXPR_GREATEREQUAL:
        print_binary(expr, ">=");
        break;

    case EXPR_SMALLEREQUAL:
        print_binary(expr, "<=");
        break;

    case EXPR_NOTEQUAL:
        print_binary(expr, "!=");
        break;

    case EXPR_UNARY_MINUS:
        print_unary(expr, "-");
        break;

    case EXPR_LOG_NOT:
        print_unary(expr, "!");
        break;

    case EXPR_BIN_NOT:
        print_unary(expr, "~");
        break;

    case EXPR_FUNCTION:
        printf("%s(", expr->function->name);
        for(struct function_param *param = expr->param; param; param = param->next) {
            print_expr(param->expr);
            if(param->next) {
                printf(", ");
            }
        }
        printf(")");
        break;
    }
}

static void do_indent(int indent)
{
    for(int i = 0; i < indent; i++) {
        printf(" ");
    }
}

void print_data_row(struct data *data_row)
{
    for(struct data *data = data_row; data; data = data->next) {
        switch(data->type) {
        case DATA_NUMBER:
            printf("%ld", data->number);
            break;

        case DATA_EXPR:
            printf("(");
            print_expr(data->expr);
            printf(")");
            break;

        case DATA_IDENT:
            printf("%c", data->ident);
            break;

        case DATA_BITS:
            printf("bits(%ld, ", data->number);
            print_expr(data->expr);
            printf(")");
            break;
        }
        if(data->next) {
            printf(" ");
        }
    }
}

void print_stmt(struct stmt *stmt, int indent)
{
    printf("%-4d ", stmt->line);
    switch(stmt->type) {
    case STMT_LET:
        do_indent(indent);
        printf("let %s = ", stmt->name);
        print_expr(stmt->expr);
        printf(";\n");
        break;

    case STMT_LOOP:
        do_indent(indent);
        printf("loop (%s,%ld)\n", stmt->name, stmt->number);
        for(struct stmt *s = stmt->stmt; s; s = s->next) {
            print_stmt(s, indent + 2);
        }
        do_indent(indent);
        printf("     end loop\n");
        break;

    case STMT_REPEAT:
        do_indent(indent);
        printf("repeat(%ld) ", stmt->number);
        print_data_row(stmt->data);
        printf("\n");
        break;

    case STMT_DATA_ROW:
        do_indent(indent);
        print_data_row(stmt->data);
        printf("\n");
        break;
    }
}

void print_file_content(struct test_file *test_file)
{
    if(!test_file) return;

    int max_len = 0;
    for(struct signal *signal = test_file->signal; signal; signal = signal->next) {
        if(strlen(signal->name) > max_len) {
            max_len = strlen(signal->name);
        }
    }

    printf("Signals:\n");
    for(struct signal *signal = test_file->signal; signal; signal = signal->next) {
        printf("%-*s ", max_len, signal->name);
        if(signal->type == SIGNAL_INPUT)  printf("INPUT  ");
        if(signal->type == SIGNAL_OUTPUT) printf("OUTPUT ");
        if(signal->type == SIGNAL_NONE)   printf("IGNORE ");
        for(struct pin *p = signal->pin; p; p = p->next) {
            printf("%d", p->number);
            if(p->next) printf(", ");
        }
        printf("\n");
    }

    printf("\nTest case:\n");
    for(struct stmt *stmt = test_file->stmt; stmt; stmt = stmt->next) {
        print_stmt(stmt, 0);
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Usage: %s file\n", argv[0]);
        return 1;
    }

    struct test_file *test_file = test_file_open(argv[1]);
    print_file_content(test_file);

    printf("\nOutput:\n");

    struct eval_context eval_ctx = {
        .up = 0,
        .variable = 0,
        .error = 0,
    };

    struct stmt *estmt = eval_stmts(test_file->stmt, &eval_ctx);
    if(eval_ctx.error) {
        printf("Error: %s\n", eval_ctx.error);
    }

    for(struct stmt *stmt = estmt; stmt; stmt = stmt->next) {
        print_stmt(stmt, 0);
    }

    free_stmt(estmt);


    test_file_close(test_file);


    return 0;
}
