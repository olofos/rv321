#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tokenizer.h"
#include "util.h"

void panic(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");

    exit(1);
}


#ifndef UNIT_TESTING
void *allocate(size_t size)
{
    void *p = calloc(size, 1);

    if(!p) {
        panic("Failed to allocate memory");
    }

    return p;
}
#else
void *test_allocate(size_t size,const char* file, const int line)
{
    void *p = _test_calloc(size, 1,file,line);

    if(!p) {
        panic("Failed to allocate memory");
    }

    return p;
}
#endif

void print_error(struct tokenizer_context *ctx, const char *fmt, ...)
{
    va_list ap;

    if(ctx) {
        fprintf(stderr, "Error on line %d: ", ctx->line + 1);
    }

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
}
