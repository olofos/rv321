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

// LCOV_EXCL_START

static int enable_wrap_fopen_fclose = 0;

FILE *__real_fopen(const char *pathname, const char *mode);
int __real_fclose(FILE *stream);

FILE *__wrap_fopen(const char *pathname, const char *mode)
{
    if(enable_wrap_fopen_fclose) {
        check_expected(pathname);
        check_expected(mode);

        return (FILE*)mock();
    } else {
        return __real_fopen(pathname, mode);
    }
}

int __wrap_fclose(FILE *stream)
{
    if(enable_wrap_fopen_fclose) {
        check_expected(stream);
        return mock();
    } else {
        return __real_fclose(stream);
    }
}

// LCOV_EXCL_STOP

static void test_file_open__should__open_and_parse_file(void **state)
{
    char *input = "a b c\n#! a input 0\n#! b input 1\n#! c output 2\n0 0 0\n";
    FILE *f = fmemopen(input, strlen(input), "r");

    expect_string(__wrap_fopen, pathname, "test.tst");
    expect_string(__wrap_fopen, mode, "r");
    will_return(__wrap_fopen, f);

    expect_value(__wrap_fclose, stream, f);
    will_return(__wrap_fclose, 0);

    struct test_file *test_file = test_file_open("test.tst");

    assert_non_null(test_file);
    assert_non_null(test_file->signal);
    assert_non_null(test_file->stmt);

    free_test_file(test_file);

    __real_fclose(f);
}

static void test_file_open__should__return_null_if_fopen_fails(void **state)
{
    expect_string(__wrap_fopen, pathname, "test.tst");
    expect_string(__wrap_fopen, mode, "r");
    will_return(__wrap_fopen, NULL);

    struct test_file *test_file = test_file_open("test.tst");

    assert_null(test_file);
}


static void test_file_open__should__return_null_if_header_is_invalid(void **state)
{
    char *input = "a b c\n#! x x\n0 0 0\n";
    FILE *f = fmemopen(input, strlen(input), "r");

    expect_string(__wrap_fopen, pathname, "test.tst");
    expect_string(__wrap_fopen, mode, "r");
    will_return(__wrap_fopen, f);

    expect_value(__wrap_fclose, stream, f);
    will_return(__wrap_fclose, 0);

    struct test_file *test_file = test_file_open("test.tst");

    assert_null(test_file);

    __real_fclose(f);
}

static void test_file_open__should__return_null_if_test_contains_syntax_error(void **state)
{
    char *input = "a b c\n#! a input 0\n#! b input 1\n#! c output 2\nlet a=1\n0 0 0\n";
    FILE *f = fmemopen(input, strlen(input), "r");

    expect_string(__wrap_fopen, pathname, "test.tst");
    expect_string(__wrap_fopen, mode, "r");
    will_return(__wrap_fopen, f);

    expect_value(__wrap_fclose, stream, f);
    will_return(__wrap_fclose, 0);

    struct test_file *test_file = test_file_open("test.tst");

    assert_null(test_file);

    __real_fclose(f);
}

static int gr_setup_wrap_fopen_fclose(void **state)
{
    enable_wrap_fopen_fclose = 1;
    return 0;
}

static int gr_teardown_wrap_fopen_fclose(void **state)
{
    enable_wrap_fopen_fclose = 0;
    return 0;
}

const struct CMUnitTest tests_for_test_file_open[] = {
    cmocka_unit_test(test_file_open__should__open_and_parse_file),
    cmocka_unit_test(test_file_open__should__return_null_if_fopen_fails),
    cmocka_unit_test(test_file_open__should__return_null_if_header_is_invalid),
    cmocka_unit_test(test_file_open__should__return_null_if_test_contains_syntax_error),
};

int main(void)
{
    int fails = 0;
    fails += cmocka_run_group_tests(tests_for_test_file_open, gr_setup_wrap_fopen_fclose, gr_teardown_wrap_fopen_fclose);

    return fails;
}
