# See LICENSE for license details.

#*****************************************************************************
# lh.S
#-----------------------------------------------------------------------------
#
# Test lh instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

#ifdef SUITE
#undef SUITE
#endif
#define SUITE 32

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_LD_OP( 2, lh, 0x00000000000000ff, 0,  tdat_lh );
  TEST_LD_OP( 3, lh, 0xffffffffffffff00, 2,  tdat_lh );
  TEST_LD_OP( 4, lh, 0x0000000000000ff0, 4,  tdat_lh );
  TEST_LD_OP( 5, lh, 0xfffffffffffff00f, 6, tdat_lh );

  # Test with negative offset

  TEST_LD_OP( 6, lh, 0x00000000000000ff, -6,  tdat_lh4 );
  TEST_LD_OP( 7, lh, 0xffffffffffffff00, -4,  tdat_lh4 );
  TEST_LD_OP( 8, lh, 0x0000000000000ff0, -2,  tdat_lh4 );
  TEST_LD_OP( 9, lh, 0xfffffffffffff00f,  0, tdat_lh4 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x00000000000000ff, \
    la  x1, tdat_lh; \
    addi x1, x1, -32; \
    lh x5, 32(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0xffffffffffffff00, \
    la  x1, tdat_lh; \
    addi x1, x1, -5; \
    lh x5, 7(x1); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_LD_DEST_BYPASS( 12, 0, lh, 0x0000000000000ff0, 2, tdat_lh2 );
  TEST_LD_DEST_BYPASS( 13, 1, lh, 0xfffffffffffff00f, 2, tdat_lh3 );
  TEST_LD_DEST_BYPASS( 14, 2, lh, 0xffffffffffffff00, 2, tdat_lh1 );

  TEST_LD_SRC1_BYPASS( 15, 0, lh, 0x0000000000000ff0, 2, tdat_lh2 );
  TEST_LD_SRC1_BYPASS( 16, 1, lh, 0xfffffffffffff00f, 2, tdat_lh3 );
  TEST_LD_SRC1_BYPASS( 17, 2, lh, 0xffffffffffffff00, 2, tdat_lh1 );

  #-------------------------------------------------------------
  # Test write-after-write hazard
  #-------------------------------------------------------------

  TEST_CASE( 18, x2, 2, \
    la  x5, tdat_lh; \
    lh  x2, 0(x5); \
    li  x2, 2; \
  )

  TEST_CASE( 19, x2, 2, \
    la  x5, tdat_lh; \
    lh  x2, 0(x5); \
    nop; \
    li  x2, 2; \
  )

  TEST_PASSFAIL

RVTEST_CODE_END

  .section .rodata
RVTEST_DATA_BEGIN

  TEST_DATA

tdat_lh:
tdat_lh1:  .half 0x00ff
tdat_lh2:  .half 0xff00
tdat_lh3:  .half 0x0ff0
tdat_lh4:  .half 0xf00f

RVTEST_DATA_END
