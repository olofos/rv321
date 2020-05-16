#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "util.h"
#include "tokenizer.h"
#include "parser.h"

void free_test_file(struct test_file *test_file)
{
    if(!test_file) return;

    free_signal(test_file->signal);
    free_stmt(test_file->stmt);
    free(test_file);
}

static struct test_file *parse_test_file(FILE *f)
{
    struct test_file *test_file = 0;
    struct signal *signal = 0;
    struct stmt *stmt = 0;

    struct tokenizer_context ctx;

    tokenizer_init(&ctx, f);

    if(!(signal = parse_header(&ctx))) {
        goto err;
    }

    if(!(stmt = parse_stmts(&ctx))) {
        goto err;
    }

    test_file = allocate(sizeof(*test_file));
    test_file->stmt = stmt;
    test_file->signal = signal;

    return test_file;

err:
    free_signal(signal);
    free_stmt(stmt);
    free(test_file);
    return 0;
}


struct test_file *test_file_open(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if(!f) {
        print_error(0, "Could not open file %s", filename);
        return 0;
    }

    struct test_file *test_file = parse_test_file(f);

    fclose(f);

    return test_file;
}
