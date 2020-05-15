#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cmocka.h>

#include "parser.h"

struct function function_tab[] = {
    { .name = 0, .nparams = 0, .func = 0 },
};

struct tokenizer_context *init_test_context(char *input)
{
    struct tokenizer_context *ctx = malloc(sizeof(*ctx));
    FILE *f = fmemopen(input, strlen(input), "r");
    tokenizer_init(ctx, f);
    return ctx;
}

void deinit_test_context(struct tokenizer_context *ctx)
{
    fclose(ctx->fp);
    free(ctx);
}

static struct tokenizer_context *test_ctx;

struct stmt *test_parse_stmt(char *input)
{
    test_ctx = init_test_context(input);
    return parse_stmt(test_ctx);
}

struct stmt *test_parse_stmts(char *input)
{
    test_ctx = init_test_context(input);
    return parse_stmts(test_ctx);
}

void teardown_test_stmt(struct stmt *stmt)
{
    free_stmt(stmt);
    deinit_test_context(test_ctx);
}


static void parse_stmt__should__parse_data_row_1(void **state)
{
    char *inputs[] = {"Z\n", "1 \n", "(1+1)\n"};

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct stmt *stmt = test_parse_stmt(inputs[0]);
        assert_non_null(stmt);
        assert_non_null(stmt->data);
        assert_null(stmt->data->next);
        teardown_test_stmt(stmt);
    }
}

static void parse_stmt__should__parse_data_row_2(void **state)
{
    char *inputs[] = {"Z 1\n", "1 x\n", "(1+1) bits(3,a)"};

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct stmt *stmt = test_parse_stmt(inputs[0]);
        assert_non_null(stmt);
        assert_non_null(stmt->data);
        assert_non_null(stmt->data->next);
        assert_null(stmt->data->next->next);
        teardown_test_stmt(stmt);
    }
}

static void parse_stmt__should__parse_let(void **state)
{
    struct stmt *stmt = test_parse_stmt("let a = 1;");
    assert_non_null(stmt);
    assert_non_null(stmt->name);
    assert_non_null(stmt->expr);
    teardown_test_stmt(stmt);
}

static void parse_stmt__should__parse_loop(void **state)
{
    struct stmt *stmt = test_parse_stmt("loop (i,8)\n1 0 1 0\nend loop");
    assert_non_null(stmt);
    assert_non_null(stmt->name);
    assert_non_null(stmt->stmt);
    assert_int_equal(stmt->number, 8);
    teardown_test_stmt(stmt);
}

static void parse_stmt__should__parse_loop_with_multiple_stmts(void **state)
{
    struct stmt *stmt = test_parse_stmt("loop (i,8)\n1 0 1 0\n0 1 0 1\n1 0 1 0\nend loop");
    assert_non_null(stmt);
    assert_non_null(stmt->name);
    assert_non_null(stmt->stmt);
    assert_int_equal(stmt->number, 8);
    teardown_test_stmt(stmt);
}

static void parse_stmt__should__parse_loop_with_extra_newlines(void **state)
{
    struct stmt *stmt = test_parse_stmt("loop (i,8)\n\n1 0 1 0\n0 1 0 1\n\n1 0 1 0\n\nend loop");
    assert_non_null(stmt);
    assert_non_null(stmt->name);
    assert_non_null(stmt->stmt);
    assert_int_equal(stmt->number, 8);
    teardown_test_stmt(stmt);
}

static void parse_stmt__should__parse_repeat(void **state)
{
    struct stmt *stmt = test_parse_stmt("repeat (8) 1 0 1 0\n");
    assert_non_null(stmt);
    assert_non_null(stmt->data);
    assert_int_equal(stmt->number, 8);
    teardown_test_stmt(stmt);
}

static void parse_stmt__should__ignore_leading_newlines(void **state)
{
    char *inputs[] = {"\n\nZ 1\n", "\n\nlet a = 1;" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct stmt *stmt = test_parse_stmt(inputs[0]);
        assert_non_null(stmt);
        teardown_test_stmt(stmt);
    }
}

static void parse_stmt__should__ignore_comments(void **state)
{
    struct stmt *stmt = test_parse_stmt("#test\n#test\n(a) 1 2\n");
    assert_non_null(stmt);
    teardown_test_stmt(stmt);
}

static void parse_stmt__should__set_next_to_null(void **state)
{
    char *inputs[] = {
        "1 1",
        "0 0 0\n",
        "repeat (3) 0 0",
        "let a = 1;",
        "loop(a,8)\n1 0 1\nend loop",
    };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct stmt *stmt = test_parse_stmt(inputs[i]);
        assert_non_null(stmt);
        assert_null(stmt->next);
        teardown_test_stmt(stmt);
    }
}

static void parse_stmt__should__return_null_for_misformed_statements(void **state)
{
    char *inputs[] = {
        "1 (1",
        "0 0 0;",
        "$",
        "repeat (a) 0 0",
        "rpt (8) 1 2 3\n",
        "repeat (8)\n0 0",
        "repeat (8);",
        "repeat (8) 0;",
        "loop (i,8)\n1 1\nend\nloop",
        "loop (8,i)\n1 1\nend loop",
        "loop (a,b)\n1 1\nend loop",
        "loop (n,8)\na\nend loop\n",
        "let a = 1\n1 2 3\n",
        "let 1 = 2;",
        "let a = $;",
        "loop (a,1)\na a\nend loop",
        "loop (a,1)\n1 1\n",
    };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct stmt *stmt = test_parse_stmt(inputs[i]);
        assert_null(stmt);
        teardown_test_stmt(stmt);
    }
}


static void parse_stmts__should__parse_all_stmts(void **state) {
    char *input =
        "let a = 1;\n"
        "loop (i,3)\n"
        "  1 0 (i)\n"
        "  0 1 X\n"
        "  bits(2,3)\n"
        "end loop\n"
        "0 0\n"
        "repeat(3) 1 1\n";

    struct stmt *stmt = test_parse_stmts(input);

    assert_non_null(stmt);
    assert_int_equal(stmt->type, STMT_LET);

    assert_non_null(stmt->next);
    assert_int_equal(stmt->next->type, STMT_LOOP);

    assert_non_null(stmt->next->next);
    assert_int_equal(stmt->next->next->type, STMT_DATA_ROW);

    assert_non_null(stmt->next->next->next);
    assert_int_equal(stmt->next->next->next->type, STMT_REPEAT);

    assert_null(stmt->next->next->next->next);

    teardown_test_stmt(stmt);
}


const struct CMUnitTest tests_for_parse_stmt[] = {
    cmocka_unit_test(parse_stmt__should__parse_data_row_1),
    cmocka_unit_test(parse_stmt__should__parse_data_row_2),
    cmocka_unit_test(parse_stmt__should__parse_let),
    cmocka_unit_test(parse_stmt__should__parse_loop),
    cmocka_unit_test(parse_stmt__should__parse_loop_with_extra_newlines),
    cmocka_unit_test(parse_stmt__should__parse_loop_with_multiple_stmts),
    cmocka_unit_test(parse_stmt__should__parse_repeat),
    cmocka_unit_test(parse_stmt__should__ignore_leading_newlines),
    cmocka_unit_test(parse_stmt__should__ignore_comments),
    cmocka_unit_test(parse_stmt__should__set_next_to_null),
    cmocka_unit_test(parse_stmt__should__return_null_for_misformed_statements),
};

const struct CMUnitTest tests_for_parse_stmts[] = {
    cmocka_unit_test(parse_stmts__should__parse_all_stmts),
};

int main(void)
{
    int fails = 0;
    fails += cmocka_run_group_tests(tests_for_parse_stmt, NULL, NULL);
    fails += cmocka_run_group_tests(tests_for_parse_stmts, NULL, NULL);

    return fails;
}
