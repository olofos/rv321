#ifndef UTIL_H_
#define UTIL_H_

// #define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

void panic(const char *fmt, ...);
void *allocate(size_t size);
void *reallocate(void *ptr, size_t size);

struct tokenizer_context;

void print_error(struct tokenizer_context *ctx, const char *fmt, ...);

#define parse_error(ctx, ...)                   \
    do {                                        \
        print_error(ctx, __VA_ARGS__);          \
        goto err;                               \
    } while(0)



#ifdef UNIT_TESTING

void *test_allocate(size_t size, const char* file, const int line);
void *test_reallocate(void *ptr, size_t size, const char* file, const int line);
void* _test_malloc(const size_t size, const char* file, const int line);
void* _test_calloc(const size_t number_of_elements, const size_t size, const char* file, const int line);
void *_test_realloc(void *ptr, size_t size, const char* file, const int line);
void _test_free(void* const ptr, const char* file, const int line);

#define test_malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define test_calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define test_free(ptr) _test_free(ptr, __FILE__, __LINE__)

/* Redirect malloc, calloc and free to the unit test allocators. */
#define malloc test_malloc
#define calloc test_calloc
#define free test_free

#define allocate(size) test_allocate(size,__FILE__,__LINE__)
#define reallocate(ptr,size) test_reallocate(ptr,size,__FILE__,__LINE__)
#endif

#endif
