# See LICENSE for license details.

#*****************************************************************************
# sh.S
#-----------------------------------------------------------------------------
#
# Test sh instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

#ifdef SUITE
#undef SUITE
#endif
#define SUITE 37

RVTEST_RV64U
RVTEST_CODE_BEGIN

  la x1, tdat_sh
  li x2, 0xbeefbeef
  sw x2, 0(x1)
  sw x2, 4(x1)
  sw x2, 8(x1)
  sw x2, 16(x1)
  sw x2, 20(x1)

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ST_OP( 2, lh, sh, 0x00000000000000aa, 0, tdat_sh );
  TEST_ST_OP( 3, lh, sh, 0xffffffffffffaa00, 2, tdat_sh );
  TEST_ST_OP( 4, lw, sh, 0xffffffffbeef0aa0, 4, tdat_sh );
  TEST_ST_OP( 5, lh, sh, 0xffffffffffffa00a, 6, tdat_sh );

  # Test with negative offset

  TEST_ST_OP( 6, lh, sh, 0x00000000000000aa, -6, tdat_sh8 );
  TEST_ST_OP( 7, lh, sh, 0xffffffffffffaa00, -4, tdat_sh8 );
  TEST_ST_OP( 8, lh, sh, 0x0000000000000aa0, -2, tdat_sh8 );
  TEST_ST_OP( 9, lh, sh, 0xffffffffffffa00a, 0,  tdat_sh8 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x5678, \
    la  x1, tdat_sh9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    sh x2, 32(x4); \
    lh x5, 0(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0x3098, \
    la  x1, tdat_sh9; \
    li  x2, 0x00003098; \
    addi x1, x1, -5; \
    sh x2, 7(x1); \
    la  x4, tdat_sh10; \
    lh x5, 0(x4); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ST_SRC12_BYPASS( 12, 0, 0, lh, sh, 0xffffffffffffccdd, 0,  tdat_sh );
  TEST_ST_SRC12_BYPASS( 13, 0, 1, lh, sh, 0xffffffffffffbccd, 2,  tdat_sh );
  TEST_ST_SRC12_BYPASS( 14, 0, 2, lh, sh, 0xffffffffffffbbcc, 4,  tdat_sh );
  TEST_ST_SRC12_BYPASS( 15, 1, 0, lh, sh, 0xffffffffffffabbc, 6, tdat_sh );
  TEST_ST_SRC12_BYPASS( 16, 1, 1, lh, sh, 0xffffffffffffaabb, 8, tdat_sh );
  TEST_ST_SRC12_BYPASS( 17, 2, 0, lh, sh, 0xffffffffffffdaab, 10, tdat_sh );

  TEST_ST_SRC21_BYPASS( 18, 0, 0, lh, sh, 0x2233, 0,  tdat_sh );
  TEST_ST_SRC21_BYPASS( 19, 0, 1, lh, sh, 0x1223, 2,  tdat_sh );
  TEST_ST_SRC21_BYPASS( 20, 0, 2, lh, sh, 0x1122, 4,  tdat_sh );
  TEST_ST_SRC21_BYPASS( 21, 1, 0, lh, sh, 0x0112, 6, tdat_sh );
  TEST_ST_SRC21_BYPASS( 22, 1, 1, lh, sh, 0x0011, 8, tdat_sh );
  TEST_ST_SRC21_BYPASS( 23, 2, 0, lh, sh, 0x3001, 10, tdat_sh );

  li a0, 0xbeef
  la a1, tdat_sh
  sh a0, 6(a1)

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

  .bss
tdat_sh:
tdat_sh1:  .skip 2
tdat_sh2:  .skip 2
tdat_sh3:  .skip 2
tdat_sh4:  .skip 2
tdat_sh5:  .skip 2
tdat_sh6:  .skip 2
tdat_sh7:  .skip 2
tdat_sh8:  .skip 2
tdat_sh9:  .skip 2
tdat_sh10: .skip 2

RVTEST_DATA_END
