#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cmocka.h>

#include "parser.h"

#define BUFSIZE 1024

number_t func_const(number_t param[])
{
    return 42;
}

number_t func_ident(number_t param[])
{
    return param[0];
}

number_t func_add(number_t param[])
{
    return param[0] + param[1];
}

struct function function_tab[] = {
    { .name = "const", .nparams = 0, .func = func_const },
    { .name = "ident", .nparams = 1, .func = func_ident },
    { .name = "add",   .nparams = 2, .func = func_add },
    { .name = 0, .nparams = 0, .func = 0 },
};

struct test_context {
    struct tokenizer_context *ctx;
    struct expr *expr;
    struct eval_context *eval_ctx;
    FILE *fp;
};

struct test_var {
    const char *name;
    number_t value;
};

static int setup(void **state)
{
    struct test_context *tctx = calloc(sizeof(*tctx), 1);
    struct tokenizer_context *ctx = malloc(sizeof(*ctx));
    struct eval_context *eval_ctx = calloc(sizeof(*eval_ctx), 1);

    tctx->ctx = ctx;
    tctx->eval_ctx = eval_ctx;
    tctx->fp = fmemopen(NULL, BUFSIZE, "r+");

    *state = tctx;
    return 0;
}

static int teardown(void **state)
{
    struct test_context *tctx = *state;

    free_expr(tctx->expr);
    free_eval_context(tctx->eval_ctx);
    free(tctx->ctx);
    free(tctx);
    return 0;
}

struct expr *test_get_expr(void **state, char *input)
{
    struct test_context *tctx = *state;

    fwrite(input, strlen(input), 1, tctx->fp);
    fseek(tctx->fp, 0, SEEK_SET);

    tokenizer_init(tctx->ctx, tctx->fp);

    tctx->expr = parse_expr(tctx->ctx);

    return tctx->expr;
}

struct eval_context *test_get_eval_context(void **state, struct test_var *vars)
{
    struct test_context *tctx = *state;

    for(struct test_var *var = vars; var && var->name; var++) {
        set_variable(tctx->eval_ctx, var->name, var->value);
    }

    return tctx->eval_ctx;
}


static void get_variable__should__get_the_value(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 42}, {.name = 0, .value = 0}};
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    number_t res = get_variable(eval_ctx, "a");

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void get_variable__should__return_error_for_unknown_variable(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 42}, {.name = 0, .value = 0}};
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    get_variable(eval_ctx, "b");

    assert_non_null(eval_ctx->error);

    teardown(state);
}

static void set_variable__should__set_the_value_if_it_does_not_exist(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 84}, {.name = 0, .value = 0}};
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    set_variable(eval_ctx, "b", 42);

    assert_null(eval_ctx->error);

    number_t res = get_variable(eval_ctx, "b");

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void set_variable__should__set_the_value_if_it_exists_1(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 84}, {.name = 0, .value = 0}};
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    set_variable(eval_ctx, "a", 42);

    assert_null(eval_ctx->error);

    number_t res = get_variable(eval_ctx, "a");

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void set_variable__should__set_the_value_if_it_exists_2(void **state)
{
    struct test_var vars[] = {{.name = "c", .value = 1}, {.name = "a", .value = 84}, {.name = "b", .value = 21}, {.name = 0, .value = 0}};
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    set_variable(eval_ctx, "a", 42);

    assert_null(eval_ctx->error);

    number_t res = get_variable(eval_ctx, "a");

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}



static void expr_eval__should__evaluate_a_number(void **state)
{
    struct expr *expr = test_get_expr(state, "42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_addition(void **state)
{
    struct expr *expr = test_get_expr(state, "20+22");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_subtraction(void **state)
{
    struct expr *expr = test_get_expr(state, "64-22");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_multiplication(void **state)
{
    struct expr *expr = test_get_expr(state, "6*7");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_division(void **state)
{
    struct expr *expr = test_get_expr(state, "294 / 7");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_and(void **state)
{
    struct expr *expr = test_get_expr(state, "111 & 186");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}


static void expr_eval__should__evaluate_or(void **state)
{
    struct expr *expr = test_get_expr(state, "34 | 8");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_xor(void **state)
{
    struct expr *expr = test_get_expr(state, "135 ^ 173");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}


static void expr_eval__should__evaluate_mod(void **state)
{
    struct expr *expr = test_get_expr(state, "4242 % 100");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_shift_left(void **state)
{
    struct expr *expr = test_get_expr(state, "21 << 1");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_shift_right(void **state)
{
    struct expr *expr = test_get_expr(state, "84 >> 1");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}


static void expr_eval__should__evaluate_equal_to_false_when_smaller(void **state)
{
    struct expr *expr = test_get_expr(state, "42 = 84");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_equal_to_false_when_greater(void **state)
{
    struct expr *expr = test_get_expr(state, "84 = 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_equal_to_true_when_equal(void **state)
{
    struct expr *expr = test_get_expr(state, "42 = 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_not_equal_to_true_when_smaller(void **state)
{
    struct expr *expr = test_get_expr(state, "42 != 84");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_not_equal_to_true_when_greater(void **state)
{
    struct expr *expr = test_get_expr(state, "84 != 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_not_equal_to_false_when_equal(void **state)
{
    struct expr *expr = test_get_expr(state, "42 != 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_smaller_to_true_when_smaller(void **state)
{
    struct expr *expr = test_get_expr(state, "42 < 84");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_smaller_to_false_when_greater(void **state)
{
    struct expr *expr = test_get_expr(state, "84 < 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_smaller_to_false_when_equal(void **state)
{
    struct expr *expr = test_get_expr(state, "42 < 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_greater_to_true_when_greater(void **state)
{
    struct expr *expr = test_get_expr(state, "84 > 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_greater_to_false_when_smaller(void **state)
{
    struct expr *expr = test_get_expr(state, "42 > 84");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_greater_to_false_when_equal(void **state)
{
    struct expr *expr = test_get_expr(state, "42 > 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}



static void expr_eval__should__evaluate_smaller_equal_to_true_when_smaller(void **state)
{
    struct expr *expr = test_get_expr(state, "42 <= 84");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_smaller_equal_to_true_when_equal(void **state)
{
    struct expr *expr = test_get_expr(state, "42 <= 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_smaller_equal_to_false_when_greater(void **state)
{
    struct expr *expr = test_get_expr(state, "84 <= 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_greater_equal_to_true_when_greater(void **state)
{
    struct expr *expr = test_get_expr(state, "84 >= 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_greater_equal_to_true_when_equal(void **state)
{
    struct expr *expr = test_get_expr(state, "42 >= 42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_greater_equal_to_false_when_smaller(void **state)
{
    struct expr *expr = test_get_expr(state, "42 >= 84");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_logical_not_of_zero(void **state)
{
    struct expr *expr = test_get_expr(state, "!0");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 1);

    teardown(state);
}

static void expr_eval__should__evaluate_logical_not_of_one(void **state)
{
    struct expr *expr = test_get_expr(state, "!1");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_logical_not_of_number(void **state)
{
    struct expr *expr = test_get_expr(state, "!42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 0);

    teardown(state);
}

static void expr_eval__should__evaluate_binary_not(void **state)
{
    struct expr *expr = test_get_expr(state, "~213 & 0xFF");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_unary_minus(void **state)
{
    struct expr *expr = test_get_expr(state, "-42");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, -42);

    teardown(state);
}

static void expr_eval__should__evaluate_function_with_zero_parameters(void **state)
{
    struct expr *expr = test_get_expr(state, "const()");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}


static void expr_eval__should__evaluate_function_with_one_parameter(void **state)
{
    struct expr *expr = test_get_expr(state, "ident(42)");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_function_with_two_parameters(void **state)
{
    struct expr *expr = test_get_expr(state, "add(20,22)");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_a_variable_1(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 42}, {.name = 0, .value = 0}};
    struct expr *expr = test_get_expr(state, "a");
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_a_variable_2(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 42}, {.name = "b", .value = 82}, {.name = 0, .value = 0}};
    struct expr *expr = test_get_expr(state, "a");
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_a_variable_3(void **state)
{
    struct test_var vars[] = {{.name = "b", .value = 82}, {.name = "a", .value = 42}, {.name = 0, .value = 0}};
    struct expr *expr = test_get_expr(state, "a");
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__evaluate_composite_expression(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 18}, {.name = 0, .value = 0}};
    struct expr *expr = test_get_expr(state, "3*8+ident(a*2)/2");
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    number_t res = eval_expr(expr, eval_ctx);

    assert_null(eval_ctx->error);
    assert_int_equal(res, 42);

    teardown(state);
}

static void expr_eval__should__return_error_for_division_by_zero(void **state)
{
    struct expr *expr = test_get_expr(state, "2 / 0");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    eval_expr(expr, eval_ctx);

    assert_non_null(eval_ctx->error);

    teardown(state);
}

static void expr_eval__should__return_error_for_mod_by_zero(void **state)
{
    struct expr *expr = test_get_expr(state, "2 % 0");
    struct eval_context *eval_ctx = test_get_eval_context(state, 0);

    eval_expr(expr, eval_ctx);

    assert_non_null(eval_ctx->error);

    teardown(state);
}

static void expr_eval__should__return_error_for_unknown_variable(void **state)
{
    struct test_var vars[] = {{.name = "a", .value = 42}, {.name = 0, .value = 0}};
    struct expr *expr = test_get_expr(state, "b");
    struct eval_context *eval_ctx = test_get_eval_context(state, vars);

    eval_expr(expr, eval_ctx);

    assert_non_null(eval_ctx->error);

    teardown(state);
}

static const struct CMUnitTest tests_for_get_variable[] = {
    cmocka_unit_test_setup(get_variable__should__get_the_value, setup),
    cmocka_unit_test_setup(get_variable__should__return_error_for_unknown_variable, setup),
};

static const struct CMUnitTest tests_for_set_variable[] = {
    cmocka_unit_test_setup(set_variable__should__set_the_value_if_it_does_not_exist, setup),
    cmocka_unit_test_setup(set_variable__should__set_the_value_if_it_exists_1, setup),
    cmocka_unit_test_setup(set_variable__should__set_the_value_if_it_exists_2, setup),
};

static const struct CMUnitTest tests_for_eval_expr[] = {
    cmocka_unit_test_setup(expr_eval__should__evaluate_a_number, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_addition, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_subtraction, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_multiplication, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_division, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_mod, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_shift_left, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_shift_right, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_and, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_or, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_xor, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_equal_to_false_when_smaller, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_equal_to_false_when_greater, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_equal_to_true_when_equal, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_not_equal_to_true_when_smaller, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_not_equal_to_true_when_greater, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_not_equal_to_false_when_equal, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_smaller_to_true_when_smaller, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_smaller_to_false_when_greater, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_smaller_to_false_when_equal, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_greater_to_true_when_greater, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_greater_to_false_when_smaller, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_greater_to_false_when_equal, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_smaller_equal_to_true_when_smaller, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_smaller_equal_to_true_when_equal, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_smaller_equal_to_false_when_greater, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_greater_equal_to_true_when_greater, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_greater_equal_to_true_when_equal, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_greater_equal_to_false_when_smaller, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_logical_not_of_zero, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_logical_not_of_one, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_logical_not_of_number, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_binary_not, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_unary_minus, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_function_with_zero_parameters, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_function_with_one_parameter, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_function_with_two_parameters, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_a_variable_1, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_a_variable_2, setup),
    cmocka_unit_test_setup(expr_eval__should__evaluate_a_variable_3, setup),

    cmocka_unit_test_setup(expr_eval__should__evaluate_composite_expression, setup),

    cmocka_unit_test_setup(expr_eval__should__return_error_for_unknown_variable, setup),
    cmocka_unit_test_setup(expr_eval__should__return_error_for_division_by_zero, setup),
    cmocka_unit_test_setup(expr_eval__should__return_error_for_mod_by_zero, setup),

};

int main(void)
{
    int fails = 0;
    fails += cmocka_run_group_tests(tests_for_get_variable, NULL, NULL);
    fails += cmocka_run_group_tests(tests_for_set_variable, NULL, NULL);
    fails += cmocka_run_group_tests(tests_for_eval_expr, NULL, NULL);

    return fails;
}
