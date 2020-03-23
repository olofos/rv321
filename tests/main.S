        .section .text.init
        .globl _start
_start:
        #include "rv32i/add.S"
	#include "rv32i/sub.S"
        #include "rv32i/and.S"
	#include "rv32i/or.S"
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

	#include "rv32i/fence_i.S"
	#include "rv32i/csr.S"

        .section .text.init

	li gp, 1
2:      j 2b
