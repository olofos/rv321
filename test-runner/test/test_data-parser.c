#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
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

struct data *test_parse_data(char *input)
{
    test_ctx = init_test_context(input);
    return parse_data(test_ctx);
}

void teardown_test_data(struct data *data)
{
    free_data(data);
    deinit_test_context(test_ctx);
}


static void should__parse_a_number(void **state)
{
    struct data *data = test_parse_data("0");

    assert_non_null(data);
    assert_int_equal(data->type, DATA_NUMBER);
    assert_int_equal(data->number, 0);

    teardown_test_data(data);
}

static void should__parse_known_identifiers(void **state)
{
    char *inputs[] = {"Z", "X", "C", "z", "x", "c" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct data *data = test_parse_data(inputs[i]);

        assert_non_null(data);
        assert_int_equal(data->type, DATA_IDENT);
        assert_int_equal(data->ident, toupper(inputs[i][0]));

        teardown_test_data(data);
    }
}

static void should__parse_bits(void **state)
{
    struct data *data = test_parse_data("bits(2,3)");

    assert_non_null(data);
    assert_int_equal(data->type, DATA_BITS);
    assert_int_equal(data->number, 2);
    assert_non_null(data->expr);

    teardown_test_data(data);
}

static void should__parse_an_expression(void **state)
{
    struct data *data = test_parse_data("(2)");

    assert_non_null(data);
    assert_int_equal(data->type, DATA_EXPR);
    assert_non_null(data->expr);

    teardown_test_data(data);
}

static void should__return_null_for_unknown_identifiers(void **state)
{
    char *inputs[] = {"A", "n", "XX", "Xx", "xX", "xxx", "Z0", "Z_0" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct data *data = test_parse_data(inputs[i]);
        assert_null(data);
        teardown_test_data(data);
    }
}

static void should__return_null_for_misformed_exprs(void **state)
{
    char *inputs[] = {"(1", "(1+)", "bits(1,1+)", "bits(1+1,2)" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct data *data = test_parse_data(inputs[i]);
        assert_null(data);
        teardown_test_data(data);
    }
}

static void should__set_next_to_null(void **state)
{
    char *inputs[] = {"1", "(1)", "bits(1,1)", "Z" };

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct data *data = test_parse_data(inputs[i]);
        assert_non_null(data);
        assert_null(data->next);
        teardown_test_data(data);
    }

}


const struct CMUnitTest tests_for_parse_data[] = {
    cmocka_unit_test(should__parse_a_number),
    cmocka_unit_test(should__parse_known_identifiers),
    cmocka_unit_test(should__parse_bits),
    cmocka_unit_test(should__parse_an_expression),
    cmocka_unit_test(should__return_null_for_unknown_identifiers),
    cmocka_unit_test(should__return_null_for_misformed_exprs),
    cmocka_unit_test(should__set_next_to_null),
};

int main(void)
{
    int fails = 0;
    fails += cmocka_run_group_tests(tests_for_parse_data, NULL, NULL);

    return fails;
}
