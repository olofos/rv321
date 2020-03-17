// See LICENSE for license details.

#ifndef _ENV_PHYSICAL_SINGLE_CORE_H
#define _ENV_PHYSICAL_SINGLE_CORE_H

//#include "../encoding.h"

//-----------------------------------------------------------------------
// Begin Macro
//-----------------------------------------------------------------------

#define RVTEST_RV64U

#if __riscv_xlen == 64
# define CHECK_XLEN li a0, 1; slli a0, a0, 31; bgez a0, 1f; RVTEST_PASS; 1:
#else
# define CHECK_XLEN li a0, 1; slli a0, a0, 31; bltz a0, 1f; RVTEST_PASS; 1:
#endif


#define EXTRA_TVEC_USER
#define EXTRA_TVEC_MACHINE
#define EXTRA_INIT
#define EXTRA_INIT_TIMER


#define RVTEST_CODE_BEGIN                       \
        .section .text.init;                    \
        li SUITENUM, SUITE

#define FAIL_PASTER2(x) fail_ ## x
#define PASS_PASTER2(x) pass_ ## x
#define FAIL_PASTER(x) FAIL_PASTER2(x)
#define PASS_PASTER(x) PASS_PASTER2(x)

#define fail FAIL_PASTER(SUITE)
#define pass PASS_PASTER(SUITE)

//-----------------------------------------------------------------------
// End Macro
//-----------------------------------------------------------------------

#define RVTEST_CODE_END        \
fail:                          \
1:      beqz TESTNUM, 1b;      \
        slli x1, SUITENUM, 16; \
	add gp, x1, TESTNUM;   \
2:  j 2b; \
pass:


//-----------------------------------------------------------------------
// Pass/Fail Macro
//-----------------------------------------------------------------------

#define SUITENUM x8
#define TESTNUM x9

#define RVTEST_PASS                             \
    li TESTNUM, 1;                              \
    li a0, 0;                                   \
2:  j 2b;

#define RVTEST_FAIL                             \
1:  beqz TESTNUM, 1b;                           \
    slli gp, SUITENUM, 16;                      \
    add gp, gp, TESTNUM;                        \
2:  j 2b;

//-----------------------------------------------------------------------
// Data Section Macro
//-----------------------------------------------------------------------

#define EXTRA_DATA

#define RVTEST_DATA_BEGIN                                               \
        EXTRA_DATA                                                      \
        .align 4

#define RVTEST_DATA_END .align 4

#endif
