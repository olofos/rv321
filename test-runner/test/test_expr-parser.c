#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cmocka.h>

#include "parser.h"

number_t func_f(number_t param[])
{
    return 0;
}

struct function function_tab[] = {
    { .name = "f0", .nparams = 0, .func = func_f },
    { .name = "f1", .nparams = 1, .func = func_f },
    { .name = "f2", .nparams = 2, .func = func_f },
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

struct expr *test_parse_expr(char *input)
{
    test_ctx = init_test_context(input);
    return parse_expr(test_ctx);
}

void teardown_test_expr(struct expr *expr)
{
    free_expr(expr);
    deinit_test_context(test_ctx);
}

static void parse_expr__should__parse_a_number(void **state)
{
    struct expr *expr = test_parse_expr("1");

    assert_non_null(expr);
    assert_int_equal(expr->type, EXPR_NUMBER);
    assert_int_equal(expr->number, 1);

    teardown_test_expr(expr);
}

static void parse_expr__should__parse_a_function_call_0(void **state)
{
    struct expr *expr = test_parse_expr("f0()");

    assert_non_null(expr);
    assert_int_equal(expr->type, EXPR_FUNCTION);
    assert_non_null(expr->function);
    assert_string_equal(expr->function->name, "f0");
    assert_null(expr->param);

    teardown_test_expr(expr);
}

static void parse_expr__should__parse_a_function_call_1(void **state)
{
    struct expr *expr = test_parse_expr("f1(1)");

    assert_non_null(expr);
    assert_int_equal(expr->type, EXPR_FUNCTION);
    assert_non_null(expr->function);
    assert_string_equal(expr->function->name, "f1");
    assert_non_null(expr->param);
    assert_null(expr->param->next);

    teardown_test_expr(expr);
}

static void parse_expr__should__parse_a_function_call_2(void **state)
{
    struct expr *expr = test_parse_expr("f2(1,2)");

    assert_non_null(expr);
    assert_int_equal(expr->type, EXPR_FUNCTION);
    assert_non_null(expr->function);
    assert_string_equal(expr->function->name, "f2");
    assert_non_null(expr->param);
    assert_non_null(expr->param->next);
    assert_null(expr->param->next->next);

    teardown_test_expr(expr);
}

static void parse_expr__should__parse_a_variable(void **state)
{
    struct expr *expr = test_parse_expr("x");

    assert_non_null(expr);
    assert_int_equal(expr->type, EXPR_VARIABLE);
    assert_string_equal(expr->name, "x");

    teardown_test_expr(expr);
}

static void parse_expr__should__parse_sums(void **state)
{
    char *inputs[] = {"1+2", "1 + 2", "a+b", "1*2+3*4", "-1+-2" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct expr *expr = test_parse_expr(inputs[i]);

        assert_non_null(expr);
        assert_int_equal(expr->type, '+');
        assert_non_null(expr->left);
        assert_non_null(expr->right);

        teardown_test_expr(expr);
    }
}

static void parse_expr__should__parse_unary_minus(void **state)
{
    char *inputs[] = {"-1", "-a", "-f1(1)", "-(a+b)" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct expr *expr = test_parse_expr(inputs[i]);

        assert_non_null(expr);
        assert_int_equal(expr->type, EXPR_UNARY_MINUS);
        assert_non_null(expr->child);

        teardown_test_expr(expr);
    }
}

static void parse_expr__should__parse_binary_not(void **state)
{
    char *inputs[] = {"~1", "~a", "~f1(1)", "~(a+b)", "~~0xFF" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct expr *expr = test_parse_expr(inputs[i]);

        assert_non_null(expr);
        assert_int_equal(expr->type, '~');
        assert_non_null(expr->child);

        teardown_test_expr(expr);
    }
}


static void parse_expr__should__return_null_for_unknown_functions(void **state)
{
    char *inputs[] = {"g(1,2)", "1+g(a,b)" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct expr *expr = test_parse_expr(inputs[i]);
        assert_null(expr);
        teardown_test_expr(expr);
    }
}

static void parse_expr__should__return_null_for_wrong_number_of_parameters_to_function(void **state)
{
    char *inputs[] = {"f0(1)", "f1()", "f0() + f0(1)" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct expr *expr = test_parse_expr(inputs[i]);
        assert_null(expr);
        teardown_test_expr(expr);
    }
}

static void parse_expr__should__return_null_for_misformed_exprs(void **state)
{
    char *inputs[] = { "1+", "*1", "1+(2+(3+4)", "1+\n2", "(a b)", "(1 2)", "a++", "f2(1,2", "^", "f2(1+," };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct expr *expr = test_parse_expr(inputs[i]);
        assert_null(expr);
        teardown_test_expr(expr);
    }
}


const struct CMUnitTest tests_for_parse_expr[] = {
    cmocka_unit_test(parse_expr__should__parse_a_number),
    cmocka_unit_test(parse_expr__should__parse_a_variable),
    cmocka_unit_test(parse_expr__should__parse_a_function_call_0),
    cmocka_unit_test(parse_expr__should__parse_a_function_call_1),
    cmocka_unit_test(parse_expr__should__parse_a_function_call_2),
    cmocka_unit_test(parse_expr__should__parse_sums),
    cmocka_unit_test(parse_expr__should__parse_unary_minus),
    cmocka_unit_test(parse_expr__should__parse_binary_not),
    cmocka_unit_test(parse_expr__should__return_null_for_unknown_functions),
    cmocka_unit_test(parse_expr__should__return_null_for_wrong_number_of_parameters_to_function),
    cmocka_unit_test(parse_expr__should__return_null_for_misformed_exprs),
};

int main(void)
{
    int fails = 0;
    fails += cmocka_run_group_tests(tests_for_parse_expr, NULL, NULL);

    return fails;
}
