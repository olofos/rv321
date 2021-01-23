        .section .text.init
        .global _start

#define USE_LCD 1
#define LCD_BASE 0x40000000

_start:
        // .option norvc

#ifdef USE_LCD
        li a1, LCD_BASE
	li a2, 0x0C
	sb a2, 0(a1)
	li a2, 0x01
	sb a2, 0(a1)

        la a0, str_running
        li a1, 0
        call puts
#endif

#if 1
        #include "rv32i/add.S"
	#include "rv32i/sub.S"
        #include "rv32i/and.S"
        #include "rv32i/or.S"
	#include "rv32i/xor.S"
        #include "rv32i/slt.S"
        #include "rv32i/sltu.S"

	#include "rv32i/addi.S"
	#include "rv32i/andi.S"
	#include "rv32i/ori.S"
	#include "rv32i/xori.S"
	#include "rv32i/slti.S"
	#include "rv32i/sltiu.S"

	#include "rv32i/lui.S"
	#include "rv32i/auipc.S"

	#include "rv32i/sll.S"
	#include "rv32i/srl.S"
	#include "rv32i/sra.S"
	#include "rv32i/slli.S"
	#include "rv32i/srli.S"
	#include "rv32i/srai.S"

	#include "rv32i/beq.S"
	#include "rv32i/bge.S"
	#include "rv32i/bgeu.S"
	#include "rv32i/blt.S"
	#include "rv32i/bltu.S"
	#include "rv32i/bne.S"

	#include "rv32i/jal.S"
	#include "rv32i/jalr.S"

	#include "rv32i/lw.S"
	#include "rv32i/lh.S"
	#include "rv32i/lhu.S"
	#include "rv32i/lb.S"
	#include "rv32i/lbu.S"

	#include "rv32i/sw.S"
	#include "rv32i/sh.S"
	#include "rv32i/sb.S"
#endif

	// #include "rv32i/fence_i.S"
        #include "rv32i/csr.S"

        // #include "rv32i/rvc.S"

        .section .text.init

#ifdef USE_LCD
        la a0, str_success
	li a1, 0
	call puts
        mv a0, TESTCOUNT
        li a1, 40
        call put_dec
        la a0, str_tests
	li a1, -1
	call puts
#endif
	li gp, 1
2:      j 2b

fail:
//real_fail:
#ifdef USE_LCD
	la a0, str_failure
	li a1, 0
	call puts
        //srli a0, TESTNUM, 16
        //li a1, 8
        //call put_hex_byte
        //mv a0, TESTNUM
        //li a1, 11
	//call put_hex_byte
        la a0, str_suite
        li a1, 40
        call puts
        srli a0, TESTNUM, 16
        li a1, -1
        call put_dec
        la a0, str_test
	li a1, -1
	call puts
        mv a0, TESTNUM
        li a1, -1
        call put_dec
#endif
        mv gp, TESTNUM
2:      j 2b

#ifdef USE_LCD
puts:   li t0, LCD_BASE
        bltz a1, 1f
        li t1, 0x80
        or t1, t1, a1
        sb t1, 0(t0)
1:      lb t1, 0(a0)
        beqz t1, 2f
        sb t1, 1(t0)
        addi a0, a0, 1
        j 1b
2:      ret

put_hex_byte:
        mv sp, ra
        mv a2, a0
        li t0, LCD_BASE
        li t1, 0x80
        or t1, t1, a1
        sb t1, 0(t0)

        srli a0, a2, 4
	call put_hex_digit

        mv a0, a2
	call put_hex_digit
        mv ra, sp
        ret

put_hex_digit:
        andi a0, a0, 0xF
        li t1, 9
        bgt a0, t1, 1f
        addi a0, a0, '0'
2:      sb a0, 1(t0)
        ret
1:      addi a0, a0, 'A'-10
        j 2b

put_dec:
        li t0, 0xFFF
        and a0, a0, t0
        li t0, LCD_BASE
        bltz a1, 1f
	li t1, 0x80
	or t1, t1, a1
	sb t1, 0(t0)

1:      li t1, 0
	li t2, 100
2:      blt a0, t2, 3f
	addi t1, t1, 1
	sub a0, a0, t2
	j 2b

3:      addi t1, t1, '0'
        sb t1, 1(t0)

        li t1, 0
	li t2, 10
2:      blt a0, t2, 3f
	addi t1, t1, 1
	sub a0, a0, t2
	j 2b

3:      addi t1, t1, '0'
	sb t1, 1(t0)
        addi t1, a0, '0'
        sb t1, 1(t0)
        ret

        .section .rodata
str_running:
        .asciz "Running test"
str_success:
	.asciz "Success         "
str_tests:
        .asciz " tests completed"
str_failure:
        .asciz "Failure         "
str_suite:
        .asciz "Suite "
str_test:
	.asciz " Test "
#endif
