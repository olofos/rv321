#include <stdio.h>
#include <stdint.h>

#include "tokenizer.h"
#include "parser.h"
#include "mt64.h"

number_t func_random(number_t param[]);
number_t func_ite(number_t param[]);

struct function function_tab[] = {
    { .name = "random", .nparams = 1, .func = func_random },
    { .name = "ite", .nparams = 3, .func = func_ite },
    { .name = 0, .nparams = 0, .func = 0 },
};

number_t func_random(number_t param[])
{
    number_t max = param[0];
    const uint64_t rand_max = 0xFFFFFFFFFFFFFFFF;

    return genrand64_int64() / ( 1 + (rand_max - max + 1) / max );
}

number_t func_ite(number_t param[])
{
    return param[0] ? param[1] : param[2];
}


int main(void)
{
    struct tokenizer_context tctx;
    struct tokenizer_context *ctx = &tctx;

    tokenizer_init(ctx, stdin);

    struct expr *expr = parse_expr(ctx);
    if(!expr) {
        printf("Parsing failed\n");
    }
    return 0;
}
