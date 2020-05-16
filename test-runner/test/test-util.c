#include <stdio.h>

// Trivial implementations of wrapped functions ////////////////////////////////

// LCOV_EXCL_START

FILE *__real_fopen(const char *pathname, const char *mode);
int __real_fclose(FILE *stream);

FILE *__wrap_fopen(const char *pathname, const char *mode) __attribute__((weak));
int __wrap_fclose(FILE *stream) __attribute__((weak));

FILE *__wrap_fopen(const char *pathname, const char *mode)
{
    return __real_fopen(pathname, mode);
}

int __wrap_fclose(FILE *stream)
{
    return __real_fclose(stream);
}

// LCOV_EXCL_STOP
