# See LICENSE for license details.

#*****************************************************************************
# sb.S
#-----------------------------------------------------------------------------
#
# Test sb instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

#ifdef SUITE
#undef SUITE
#endif
#define SUITE 38

RVTEST_RV64U
RVTEST_CODE_BEGIN

  la x1, tdat_sb1
  li x2, 0xefefefef
  sw x2, 0(x1)
  sw x2, 4(x1)
  sw x2, 8(x1)

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ST_OP( 2, lb, sb, 0xffffffffffffffaa, 0, tdat_sb );
  TEST_ST_OP( 3, lb, sb, 0x0000000000000000, 1, tdat_sb );
  TEST_ST_OP( 4, lh, sb, 0xffffffffffffefa0, 2, tdat_sb );
  TEST_ST_OP( 5, lb, sb, 0x000000000000000a, 3, tdat_sb );

  # Test with negative offset

  TEST_ST_OP( 6, lb, sb, 0xffffffffffffffaa, -3, tdat_sb8 );
  TEST_ST_OP( 7, lb, sb, 0x0000000000000000, -2, tdat_sb8 );
  TEST_ST_OP( 8, lb, sb, 0xffffffffffffffa0, -1, tdat_sb8 );
  TEST_ST_OP( 9, lb, sb, 0x000000000000000a, 0,  tdat_sb8 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x78, \
    la  x1, tdat_sb9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    sb x2, 32(x4); \
    lb x5, 0(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0xffffffffffffff98, \
    la  x1, tdat_sb9; \
    li  x2, 0x00003098; \
    addi x1, x1, -6; \
    sb x2, 7(x1); \
    la  x4, tdat_sb10; \
    lb x5, 0(x4); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ST_SRC12_BYPASS( 12, 0, 0, lb, sb, 0xffffffffffffffdd, 0, tdat_sb );
  TEST_ST_SRC12_BYPASS( 13, 0, 1, lb, sb, 0xffffffffffffffcd, 1, tdat_sb );
  TEST_ST_SRC12_BYPASS( 14, 0, 2, lb, sb, 0xffffffffffffffcc, 2, tdat_sb );
  TEST_ST_SRC12_BYPASS( 15, 1, 0, lb, sb, 0xffffffffffffffbc, 3, tdat_sb );
  TEST_ST_SRC12_BYPASS( 16, 1, 1, lb, sb, 0xffffffffffffffbb, 4, tdat_sb );
  TEST_ST_SRC12_BYPASS( 17, 2, 0, lb, sb, 0xffffffffffffffab, 5, tdat_sb );

  TEST_ST_SRC21_BYPASS( 18, 0, 0, lb, sb, 0x33, 0, tdat_sb );
  TEST_ST_SRC21_BYPASS( 19, 0, 1, lb, sb, 0x23, 1, tdat_sb );
  TEST_ST_SRC21_BYPASS( 20, 0, 2, lb, sb, 0x22, 2, tdat_sb );
  TEST_ST_SRC21_BYPASS( 21, 1, 0, lb, sb, 0x12, 3, tdat_sb );
  TEST_ST_SRC21_BYPASS( 22, 1, 1, lb, sb, 0x11, 4, tdat_sb );
  TEST_ST_SRC21_BYPASS( 23, 2, 0, lb, sb, 0x01, 5, tdat_sb );

  li a0, 0xef
  la a1, tdat_sb
  sb a0, 3(a1)

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

.bss
tdat_sb:
tdat_sb1:  .skip 1
tdat_sb2:  .skip 1
tdat_sb3:  .skip 1
tdat_sb4:  .skip 1
tdat_sb5:  .skip 1
tdat_sb6:  .skip 1
tdat_sb7:  .skip 1
tdat_sb8:  .skip 1
tdat_sb9:  .skip 1
tdat_sb10: .skip 1
           .skip 2

RVTEST_DATA_END
