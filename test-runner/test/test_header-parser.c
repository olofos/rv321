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

struct signal *test_parse_header(char *input)
{
    test_ctx = init_test_context(input);
    return parse_header(test_ctx);
}

void teardown_test_signal(struct signal *signal)
{
    free_signal(signal);
    deinit_test_context(test_ctx);
}

static void parse_header__should__parse_header_with_signals(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 0A0\n"
        "#! B output 0A1 0A2\n"
        "#! C ignore\n"
        "0 0 0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_INPUT);
    assert_non_null(signal->pin);

    assert_non_null(signal->next);
    assert_int_equal(signal->next->type, SIGNAL_OUTPUT);
    assert_non_null(signal->next->pin);

    assert_non_null(signal->next->next);
    assert_int_equal(signal->next->next->type, SIGNAL_NONE);
    assert_null(signal->next->next->pin);

    assert_null(signal->next->next->next);

    teardown_test_signal(signal);
}

static void parse_header__should__parse_header_with_signal_flags(void **states)
{
    char *input =
        "A B C\n"
        "#! A input[pu] 0A0\n"
        "#! B INPUT[PU] 0A1\n"
        "0 0 0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_INPUT | SIGNAL_PULLUP);
    assert_non_null(signal->pin);

    assert_non_null(signal->next);
    assert_int_equal(signal->next->type, SIGNAL_INPUT | SIGNAL_PULLUP);

    teardown_test_signal(signal);
}

static void parse_header__should__parse_signals(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 0A0\n"
        "#! B output 1B1 1B2\n"
        "#! C input 4A7 4B7\n"
        "0 0 0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_INPUT);
    assert_non_null(signal->pin);
    assert_null(signal->pin->next);

    assert_int_equal(signal->pin->bank, 0);
    assert_int_equal(signal->pin->number, 0);

    assert_non_null(signal->next);
    assert_int_equal(signal->next->type, SIGNAL_OUTPUT);
    assert_non_null(signal->next->pin);
    assert_non_null(signal->next->pin->next);
    assert_null(signal->next->pin->next->next);

    assert_int_equal(signal->next->pin->bank, (1 << 1) | 1);
    assert_int_equal(signal->next->pin->number, 1);

    assert_int_equal(signal->next->pin->next->bank, (1 << 1) | 1);
    assert_int_equal(signal->next->pin->next->number, 2);

    assert_non_null(signal->next->next);
    assert_int_equal(signal->next->next->type, SIGNAL_INPUT);
    assert_non_null(signal->next->next->pin);
    assert_non_null(signal->next->next->pin->next);
    assert_null(signal->next->next->pin->next->next);

    assert_int_equal(signal->next->next->pin->bank, (4 << 1) | 0);
    assert_int_equal(signal->next->next->pin->number, 7);

    assert_int_equal(signal->next->next->pin->next->bank, (4 << 1) | 1);
    assert_int_equal(signal->next->next->pin->next->number, 7);

    assert_null(signal->next->next->next);

    teardown_test_signal(signal);
}

static void parse_header__should__parse_skipped_signals(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 0A0 X 0A1\n"
        "#! B output 0B0 X[2] 0B1\n"
        "#! C output x 1A0\n"
        "0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_INPUT);
    assert_non_null(signal->pin);
    assert_non_null(signal->pin->next);
    assert_non_null(signal->pin->next->next);
    assert_null(signal->pin->next->next->next);

    assert_int_equal(signal->pin->bank, 0);
    assert_int_equal(signal->pin->number, 0);

    assert_int_equal(signal->pin->next->bank, PIN_SKIP);
    assert_int_equal(signal->pin->next->number, PIN_SKIP);

    assert_int_equal(signal->pin->next->next->bank, 0);
    assert_int_equal(signal->pin->next->next->number, 1);

    assert_non_null(signal->next);

    assert_int_equal(signal->next->pin->bank, 1);
    assert_int_equal(signal->next->pin->number, 0);

    assert_int_equal(signal->next->pin->next->bank, PIN_SKIP);
    assert_int_equal(signal->next->pin->next->number, PIN_SKIP);

    assert_int_equal(signal->next->pin->next->next->bank, PIN_SKIP);
    assert_int_equal(signal->next->pin->next->next->number, PIN_SKIP);

    assert_int_equal(signal->next->pin->next->next->next->bank, 1);
    assert_int_equal(signal->next->pin->next->next->next->number, 1);

    assert_non_null(signal->next->next);

    assert_int_equal(signal->next->next->pin->bank, PIN_SKIP);
    assert_int_equal(signal->next->next->pin->number, PIN_SKIP);

    assert_int_equal(signal->next->next->pin->next->bank, 2);
    assert_int_equal(signal->next->next->pin->next->number, 0);


    teardown_test_signal(signal);
}

static void parse_header__should__parse_header_without_signals(void **states)
{
    char *input =
        "A B C\n"
        "0 0 0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_NONE);
    assert_null(signal->pin);

    assert_non_null(signal->next);
    assert_int_equal(signal->next->type, SIGNAL_NONE);
    assert_null(signal->next->pin);

    assert_non_null(signal->next->next);
    assert_int_equal(signal->next->next->type, SIGNAL_NONE);
    assert_null(signal->next->next->pin);

    assert_null(signal->next->next->next);

    teardown_test_signal(signal);
}

static void parse_header__should__parse_really_long_signal_names(void **states)
{
    char *input =
        "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\n"
        "#! 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF input 1A1\n"
        "0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_INPUT);
    assert_non_null(signal->pin);
    assert_null(signal->next);

    teardown_test_signal(signal);
}

static void parse_header__should__accept_weird_signal_names(void **states)
{
    char *input =
        "!)(+=&^%£öäå\n"
        "#! !)(+=&^%£öäå input 1A1\n"
        "0\n";

    struct signal *signal = test_parse_header(input);

    assert_non_null(signal);
    assert_int_equal(signal->type, SIGNAL_INPUT);
    assert_non_null(signal->pin);
    assert_null(signal->next);

    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_eof_in_signal_list(void **states)
{
    char *input =
        "A B C";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_eof_in_pin_mapping(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 1";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_eof_after_pin_mapping(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 1A1\n";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}


static void parse_header__should__return_null_for_missing_io_pin_number(void **states)
{
    char *input =
        "A B C\n"
        "#! A input\n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_ignored_pin_number(void **states)
{
    char *input =
        "A B C\n"
        "#! A ignore 1A1\n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_unknown_signal_type(void **states)
{
    char *input =
        "A B C\n"
        "#! A buzz 1A1\n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_duplicate_signal_name(void **states)
{
    char *input =
        "A B A C\n"
        "#! A input 1A1\n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_unknown_signal_mapping(void **states)
{
    char *input =
        "A B C\n"
        "#! D input 1A1\n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_duplicate_pin_mapping(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 1A1\n"
        "#! A input 2A2\n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_empty_mapping(void **states)
{
    char *input =
        "A B C\n"
        "#! A input 1A1\n"
        "#! \n"
        "0 0 0";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_empty_file(void **states)
{
    char *input = " ";

    struct signal *signal = test_parse_header(input);
    assert_null(signal);
    teardown_test_signal(signal);
}

static void parse_header__should__return_null_for_misformed_pin_mappings(void **states)
{
    char *inputs[] = {
        "a b c\n"
        "#! a input w\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 1c0\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 9a0\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 1a8\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 1a0 xx\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 1a0 x[-1]\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 1a0 x[]\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input 1a0 x[2]]\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input[ 1a0\n"
        "0 0 0\n",

        "a b c\n"
        "#! a input[pu]] 1a0\n"
        "0 0 0\n",
};

    for(int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        struct signal *signal = test_parse_header(inputs[i]);
        assert_null(signal);
        teardown_test_signal(signal);
    }
}

const struct CMUnitTest tests_for_parse_header[] = {
    cmocka_unit_test(parse_header__should__parse_header_with_signals),
    cmocka_unit_test(parse_header__should__parse_header_with_signal_flags),
    cmocka_unit_test(parse_header__should__parse_signals),
    cmocka_unit_test(parse_header__should__parse_skipped_signals),
    cmocka_unit_test(parse_header__should__parse_header_without_signals),
    cmocka_unit_test(parse_header__should__parse_really_long_signal_names),
    cmocka_unit_test(parse_header__should__accept_weird_signal_names),
    cmocka_unit_test(parse_header__should__return_null_for_eof_in_signal_list),
    cmocka_unit_test(parse_header__should__return_null_for_eof_in_pin_mapping),
    cmocka_unit_test(parse_header__should__return_null_for_eof_after_pin_mapping),
    cmocka_unit_test(parse_header__should__return_null_for_missing_io_pin_number),
    cmocka_unit_test(parse_header__should__return_null_for_ignored_pin_number),
    cmocka_unit_test(parse_header__should__return_null_for_unknown_signal_type),
    cmocka_unit_test(parse_header__should__return_null_for_duplicate_signal_name),
    cmocka_unit_test(parse_header__should__return_null_for_unknown_signal_mapping),
    cmocka_unit_test(parse_header__should__return_null_for_duplicate_pin_mapping),
    cmocka_unit_test(parse_header__should__return_null_for_empty_mapping),
    cmocka_unit_test(parse_header__should__return_null_for_empty_file),
    cmocka_unit_test(parse_header__should__return_null_for_misformed_pin_mappings),
};

int main(void)
{
    int fails = 0;
    fails += cmocka_run_group_tests(tests_for_parse_header, NULL, NULL);

    return fails;
}
